#ifndef JSONINFO_HH
#define JSONINFO_HH

#include <string>
#include "json.hpp"

//#define DEBUG_FILE_IO
#define INVALID_LOCATION -1
#define BLOCK_SIZE 256

struct jsoninfo {
    nlohmann::json* j;
    long int doc_start;
    int index;
    int allocated_size;
    std::string name;

    inline int is_deleted() {
        return allocated_size < 0;
    }

    inline void set_deleted() {
        if (allocated_size > 0) {
            allocated_size *= -1;
        }
    }

    inline void set_undeleted() {
        if (allocated_size < 0) {
            allocated_size *= -1;
        }
    }
};

#endif
