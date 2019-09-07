#include "fileheader.hh"
#include "doc_table.hh"
#include <iostream>

int main(int argc, char *argv[]) {
    // Open file
    FILE *file = fopen("test", "w+");
    if (file == NULL) {
        std::cerr << "Error: unable to open file:\n    " << "test" << "\n";
        return -1;
    }

    // Check read-write on num_docs and header_len
    Fileheader header; 
    header.write_header(file);
    header.read_header(file);
    assert(header.num_docs == 0);
    assert(header.header_len == DEFAULT_HEADER_LEN);
    header.set_num_docs(2, file);
    header.set_header_len(50, file);
    header.read_header(file);
    assert(header.num_docs == 2);
    assert(header.header_len == 50);

    DocTable table;
    table.add({nullptr, 150, DOC_TABLE_START, 200, "test_doc_1"}, file);
    table.add({nullptr, 250, DOC_TABLE_START + sizeof(int) + sizeof(char) + 10, 200, "more_testing"}, file);
    table.modify("test_doc_1", {nullptr, 450, 50, 200, "doc_test_1"}, file);
    table.read(2, file);
    // TODO: assertions

    return 0;
}
