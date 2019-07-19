#ifndef UTILS_HH
#define UTILS_HH

#include <string>

inline std::string get_first_word(std::string s) {
    size_t n = s.find(" ");
    if (n == std::string::npos) {
        return s;
    }
    return s.substr(0, n);
}

inline std::string get_tail(std::string s) {
    // TODO: deal with unexpected whitespace
    size_t n = s.find(" ");
    if (n == std::string::npos || n == s.length() - 1) {
        return "";
    }
    return s.substr(n + 1, s.length());
}

// Returns true if the property does not start with "__"
inline bool check_property_valid(std::string property) {
    return (property.substr(0, 2) != "__");
}

inline void invalid_property_message(std::string property) {
    std::cerr << "Error: Property names beginning in \"__\" are not "
              << "allowed:\n    " << property;
}

inline bool is_valid_filename(std::string name) {
    for (unsigned int i = 0; i < name.size(); ++i) {
        if(name[i] == ' ' && (i == 0 || name[i-1] != '\\')) {
            return false;
        }
    }
    return true;
}


#endif
