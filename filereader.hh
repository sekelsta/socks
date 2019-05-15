#ifndef FILEREADER_HH
#define FILEREADER_HH

#include <vector>
#include "json.hpp"


class Filereader {
    FILE *file;
    int num_docs;
    
    std::vector<nlohmann::json*> documents;

    nlohmann::json *find_by_name(std::string name);

    public:
    void write(nlohmann::json *j);

    void create(std::string name);

    void edit(std::string name, std::string property, std::string value);

    void view(std::string name, std::string property);

    void del(std::string name);

    void open(std::string name);
};

#endif
