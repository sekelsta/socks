#include "fileheader.hh"
#include "file_utils.hh"
#include <iostream>
//#define DEBUG_HEADER

void Fileheader::set_header_len(int num, FILE *file) {
    header_len = num;
#ifdef DEBUG_HEADER
    std::cout << "Setting header length to " << num << "\n";
#endif
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
#ifdef DEBUG_HEADER
    std::cout << "Setting the number of documents to " << num << "\n";
#endif
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
#ifdef DEBUG_HEADER
    std::cout << "Reading header: length " << header_len << ", " << num_docs << "documents\n";
#endif
}

// Write current values to the file
void Fileheader::write_header(FILE *file) {
    set_header_len(header_len, file);
    set_num_docs(num_docs, file);
}
