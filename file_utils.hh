#ifndef FILE_UTILS_HH
#define FILE_UTILS_HH

#include <cstdio>

using namespace std;

inline void file_shift(int start, int new_start, FILE *file) {
    assert(new_start > start);
    // Allocate and set string end
    char *buffer_first = (char *)malloc(new_start - start + 1);
    buffer_first[new_start - start] = '\0';
    char *buffer_second = (char *)malloc(new_start - start + 1);
    buffer_second[new_start - start] = '\0';

    // Seek to start
    if (fseek(file, start, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    // Read into first buffer
    int bytes_read = fread(buffer_first, sizeof(char), new_start - start, file);
    if (bytes_read != new_start - start && !feof(file)) {
        throw "TODO: deal with file errors";
    }
    
    int read_location = start + bytes_read;
    int write_location = new_start;
    while (!feof(file)) {
        // Read into second buffer
        if (fseek(file, read_location, SEEK_SET) != 0) {
            throw "TODO: deal with file errors";
        }
        bytes_read = fread(buffer_first, sizeof(char), new_start - start, file);
        if (bytes_read != new_start - start && !feof(file)) {
            throw "TODO: deal with file errors";
        }
        read_location += bytes_read;
        // Write from first buffer
        if (fseek(file, write_location, SEEK_SET) != 0) {
            throw "TODO: deal with file errors";
        }
        int bytes_written = fwrite(buffer_first, sizeof(char), new_start - start, file);
        if (bytes_written != new_start - start) {
            throw "TODO: deal with file errors";
        }
        write_location += bytes_written;
        // Overwrite first buffer with second
        char *temp = buffer_first;
        buffer_first = buffer_second;
        buffer_second = temp;
    }
    // One last write
    if (fseek(file, write_location, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    int bytes_written = fwrite(buffer_first, sizeof(char), bytes_read, file);
    if (bytes_written != bytes_read) {
        throw "TODO: deal with file errors";
    }
           
}

#endif
