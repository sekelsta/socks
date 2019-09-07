#ifndef JSONINFO_HH
#define JSONINFO_HH

#include <string>
#include "json.hpp"

#define INVALID_LOCATION -1
#define BLOCK_SIZE 256

struct jsoninfo {
    nlohmann::json* j;
    long int doc_start;
    long int header_start;
    unsigned int allocated_size;
    std::string name;
};

#endif
