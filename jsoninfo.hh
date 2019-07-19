#ifndef JSONINFO_HH
#define JSONINFO_HH

#include <string>
#include "json.hpp"

struct jsoninfo {
    nlohmann::json* j;
    int doc_start;
    int header_start;
    unsigned int allocated_size;
    std::string name;
};

#endif
