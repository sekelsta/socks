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

    void extend_header();

public:
    inline bool is_open() {
        return file != NULL;
    }

    void write(jsoninfo *js);

    void create(std::string name);

private:
    bool is_property_hidden(std::string property);

    jsoninfo *get_json(std::string name);

public:
    void edit(std::string name, std::string property, std::string value);

    void view(std::string name, std::string property);

    void view_properties(std::string name);

    void del(std::string name);

    void open(std::string name);

    void list(std::vector<std::string> properties);
};

#endif
