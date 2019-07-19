#ifndef FILEREADER_HH
#define FILEREADER_HH

#include <vector>
#include <cstdio>
#include "jsoninfo.hh"
#include "fileheader.hh"

#define INVALID_LOCATION -1
#define BLOCK_SIZE 256


class Filereader {
    FILE *file = NULL;
    Fileheader header; 

    // Position the next document aded should start at
    int end;

    std::vector<jsoninfo> documents;

    jsoninfo *find_by_name(std::string name);

    void set_num_docs(int num);

    int get_doc_start(std::string name);

    void parse_fail();

    void extend(jsoninfo *js, int size_needed);

    void extend_header(int size_needed);

    void read_json(jsoninfo *js);

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
