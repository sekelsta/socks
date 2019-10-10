#ifndef PARSER_HH
#define PARSER_HH

#include "filereader.hh"

class Parser {
    Filereader f;
public:
    // Returns false to quit, true otherwise
    bool do_command(std::string line, bool prompt);
};

#endif
