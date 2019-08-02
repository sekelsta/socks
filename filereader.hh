#ifndef FILEREADER_HH
#define FILEREADER_HH

#include <vector>
#include <cstdio>
#include "jsoninfo.hh"
#include "fileheader.hh"
#include "doc_table.hh"


class Filereader {
    FILE *file = NULL;
    Fileheader header; 
    DocTable table;

    // Position the next document added should start at
    int end;

    int get_doc_start(std::string name);

    void parse_fail();

    void extend(jsoninfo *js, int size_needed);

    void extend_header(int size_needed);

    public:
    inline bool is_open() {
        return file != NULL;
    }

    void write(jsoninfo *js);

    void create(std::string name);

    void edit(std::string name, std::string property, std::string value);

    void view(std::string name, std::string property);

    void del(std::string name);

    void open(std::string name);
};

#endif
