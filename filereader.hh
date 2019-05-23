#ifndef FILEREADER_HH
#define FILEREADER_HH

#include <vector>
#include "json.hpp"
#include "jsoninfo.hh"

// The default amount of space allocated for the header
#define HEADER_LEN 256
#define INVALID_LOCATION -1


class Filereader {
    FILE *file = NULL;
    int num_docs = 0;
    // 1 * HEADER_LEN
    int header_len = 1;    

    std::vector<jsoninfo> documents;

    jsoninfo *find_by_name(std::string name);

    void set_num_docs(int num);

    int get_doc_start(std::string name);

    void parse_fail();

    void extend(jsoninfo *js, int size_needed);

    void read_json(jsoninfo *js);

    public:
    inline bool is_open() {
        return file != NULL;
    }

    void write(jsoninfo *js, int allocated_size);

    void create(std::string name);

    void edit(std::string name, std::string property, std::string value);

    void view(std::string name, std::string property);

    void del(std::string name);

    void open(std::string name);
};

#endif
