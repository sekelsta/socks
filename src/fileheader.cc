#include "fileheader.hh"
#include "file_utils.hh"

void Fileheader::set_header_len(int num, FILE *file) {
    header_len = num;
    // First int is header len, next int is num_docs
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    if (fwrite(&header_len, sizeof(int), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
}

void Fileheader::set_num_docs(int num, FILE *file) {
    num_docs = num;
    // First int is header len, next int is num_docs
    if (fseek(file, sizeof(int), SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    if (fwrite(&num_docs, sizeof(int), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
}


// Set fields based on the file.
void Fileheader::read_header(FILE *file) {
    // Go to the beginning of the file
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    if (fread(&num_docs, sizeof(int), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
    if (fread(&num_docs, sizeof(int), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
}

// Write current values to the file
void Fileheader::write_header(FILE *file) {
    set_header_len(header_len, file);
    set_num_docs(num_docs, file);
}
