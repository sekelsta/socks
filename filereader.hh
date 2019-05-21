#ifndef FILEREADER_HH
#define FILEREADER_HH

#include <vector>
#include "json.hpp"

// The default amount of space allocated for the header
#define HEADER_LEN 256


class Filereader {
    FILE *file = NULL;
    int num_docs = 0;
    // 1 * HEADER_LEN
    int header_len = 1;    

    std::vector<nlohmann::json*> documents;

    nlohmann::json *find_by_name(std::string name);

    int get_doc_start(std::string name);

    public:
    inline bool is_open() {
        return file != NULL;
    }

    void write(nlohmann::json *j);

    void create(std::string name);

    void edit(std::string name, std::string property, std::string value);

    void view(std::string name, std::string property);

    void del(std::string name);

    void open(std::string name);
};

#endif
