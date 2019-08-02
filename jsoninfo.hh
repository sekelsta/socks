#ifndef JSONINFO_HH
#define JSONINFO_HH

#include <string>
#include "json.hpp"

#define INVALID_LOCATION -1
#define BLOCK_SIZE 256

struct jsoninfo {
    nlohmann::json* j;
    int doc_start;
    int header_start;
    unsigned int allocated_size;
    std::string name;
};

#endif
