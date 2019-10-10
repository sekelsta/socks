#ifndef DOC_TABLE_HH
#define DOC_TABLE_HH

#include <vector>
#include <cstdio>
#include <iostream>
#include "jsoninfo.hh"
#include "fileheader.hh"

// For doing low level file IO things
class DocTable {
private:    
    std::vector<jsoninfo> documents;
    long end = DOC_TABLE_START;
    FILE * file;

    void seek_to_header_start(jsoninfo &doc);
    
public: // TODO: make private
    // Check if it exists in the documents vecor
    // Document names must be unique
    jsoninfo *find_by_name(std::string name);

public:
    void set_file(FILE *newfile);
    int get_entry_size();
    std::vector<jsoninfo> get_documents();
    // Return 0 on success, -1 if out of room
    int add(jsoninfo &doc, Fileheader &header);
    void write(jsoninfo &doc);
    void remove(std::string name);
    void modify(std::string name, jsoninfo doc);
    void modify(int index, jsoninfo doc);
    int get_doc_start(int index);
    int get_doc_start(std::string name);
    bool is_name_used(std::string name);
    void read_json(jsoninfo *js);
    void read(int num_docs);
    nlohmann::json *get_json(std::string name);
    int get_file_end();
    void shift(int start, int new_start);
    void free_jsons();
    void close();
};

#endif
