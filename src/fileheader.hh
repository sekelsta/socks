#ifndef FILEHEADER_HH
#define FILEHEADER_HH

#include <cstdio>

// The default amount of space allocated for the header
#define DEFAULT_HEADER_LEN 256
// Where the header info from here ends and the table of documents begins
#define DOC_TABLE_START 2 * sizeof(int32_t)

struct Fileheader {
    // The number of documents within the file
    int num_docs = 0;
    // The size of the header, including the table of documents
    int header_len = DEFAULT_HEADER_LEN;

    void set_header_len(int num, FILE *file);
    void set_num_docs(int n, FILE *file);
    // Set fields based on the file
    void read_header(FILE *file);
    // Write current values to the file
    void write_header(FILE *file);
};
// TODO: handle being given a file of the wrong type 
// (add an identifier to the header and use an extension)
#endif
