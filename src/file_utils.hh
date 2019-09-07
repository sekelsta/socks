#ifndef FILE_UTILS_HH
#define FILE_UTILS_HH

#include <cstdio>
#include <cassert>
#include <cstdlib>

using namespace std;

#define FILE_IO_ERROR "An error occurred during file IO: "
#define UNEXPECTED_EOF_ERROR "Encountered an unexpected end of file while reading.\n"

inline void file_shift(int start, int new_start, FILE *file) {
    assert(new_start > start);
    assert (file != nullptr);
    // Allocate and set string end
    char *buffer_first = (char *)std::malloc(new_start - start + 1);
    buffer_first[new_start - start] = '\0';
    char *buffer_second = (char *)std::malloc(new_start - start + 1);
    buffer_second[new_start - start] = '\0';

    // Seek to start
    if (fseek(file, start, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    // Read into first buffer
    int bytes_read = fread(buffer_first, sizeof(char), new_start - start, file);
    if (bytes_read != new_start - start && !feof(file)) {
        perror(FILE_IO_ERROR);
    }
    
    int read_location = start + bytes_read;
    int write_location = new_start;
    bool done = feof(file);
    while (!done) {
        // Read into second buffer
        if (fseek(file, read_location, SEEK_SET) != 0) {
            perror(FILE_IO_ERROR);
        }
        bytes_read = fread(buffer_second, sizeof(char), new_start - start, file);
        if (bytes_read != new_start - start && !feof(file)) {
            perror(FILE_IO_ERROR);
        }
        done = feof(file);
        read_location += bytes_read;
        // Write from first buffer
        if (fseek(file, write_location, SEEK_SET) != 0) {
            perror(FILE_IO_ERROR);
        }
        int bytes_written = fwrite(buffer_first, sizeof(char), new_start - start, file);
        if (bytes_written != new_start - start) {
            perror(FILE_IO_ERROR);
        }
        write_location += bytes_written;
        // Overwrite first buffer with second
        char *temp = buffer_first;
        buffer_first = buffer_second;
        buffer_second = temp;
    }
    // One last write
    if (fseek(file, write_location, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    int bytes_written = fwrite(buffer_first, sizeof(char), bytes_read, file);
    if (bytes_written != bytes_read) {
        perror(FILE_IO_ERROR);
    }
           
}

#endif
