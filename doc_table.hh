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
    
public: // TODO: make private
    // Check if it exists in the documents vecor
    // Document names must be unique
    jsoninfo *find_by_name(std::string name, FILE *file);

public:
    std::vector<jsoninfo> get_documents();
    // Return 0 on success, -1 if out of room
    int add(jsoninfo &doc, int header_len, FILE *file);
    void write(jsoninfo &doc, FILE *file);
    void remove(std::string name, FILE *file);
    void modify(std::string name, jsoninfo doc, FILE *file);
    int get_doc_start(std::string name, FILE *file);
    bool is_name_used(std::string name, FILE *file);
    void read_json(jsoninfo *js, FILE *file);
    void read(int num_docs, FILE *file);
    nlohmann::json *get_json(std::string name, FILE *file);
    int get_file_end();
    void shift(int start, int new_start, FILE *file);
};

#endif
