#include "fileheader.hh"

void Fileheader::set_header_len(int num, FILE *file) {
    header_len = num;
    // First int is header len, next int is num_docs
    if (fseek(file, 0, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    if (fwrite(&header_len, sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
}

void Fileheader::set_num_docs(int num, FILE *file) {
    num_docs = num;
    // First int is header len, next int is num_docs
    if (fseek(file, sizeof(int), SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    if (fwrite(&num_docs, sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
}


// Set fields based on the file. Return -1 on error.
void Fileheader::read_header(FILE *file) {
    if (fread(&num_docs, sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
    if (fread(&num_docs, sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
}

// Write current values to the file
void Fileheader::write_header(FILE *file) {
    set_header_len(header_len, file);
    set_num_docs(num_docs, file);
}
