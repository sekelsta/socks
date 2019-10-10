#include "parser.hh"
#include "json.hpp"
#include "utils.hh"
#include <fstream>
#include <string>
#include <vector>

using json = nlohmann::json;

bool Parser::do_command(std::string line, bool prompt) {
    // Quit
    if (to_upper(line) == "QUIT") {
        return false;
    }
    else if (to_upper(get_first_word(line)) == "OPEN") {
        if (is_valid_filename(get_tail(line))) {
            f.open(get_tail(line));
        }
        else {
            std::cerr << "Error: Must be a valid file name:\n    \""
                      << get_tail(line) << "\"\n";
        }
    }
    else if (to_upper(get_first_word(line)) == "HELP") {
        std::cout << "Supported commands are:\n"
                  << "QUIT\n"
                  << "OPEN filename\n"
                  << "CREATE name\n"
                  << "DELETE name\n"
                  << "EDIT name property value\n"
                  << "VIEW name [property]\n"
                  << "LIST [property] [next_property] ...\n"
                  << "HELP\n\n";
    }
    // Need to have an open document before doing anything else
    else if (!f.is_open()) {
        std::cerr << "Error: Must have an open document. Use:\n"
            << "    OPEN filename\n";
    }
    // Create a document
    else if (to_upper(get_first_word(line)) == "CREATE") {
        if (get_tail(line) != "" 
                && get_tail(line).find(" ") == std::string::npos) {
            f.create(get_tail(line));
        }
        else {
            std::cerr << "Error: Name cannot contain a space:\n    \""
                      << get_tail(line) << "\"\n";
        }
    }
    // Delete a document
    else if (to_upper(get_first_word(line)) == "DELETE") {
        if (get_tail(line) != "" 
                && get_tail(line).find(" ") == std::string::npos) {
            f.del(get_tail(line));
        }
        else {
            std::cerr << "Error: Name cannot contain a space:\n    \""
                      << get_tail(line) << "\"\n";
        }
    }
    // Edit a document
    else if (to_upper(get_first_word(line)) == "EDIT") {
        std::string remaining = get_tail(line);
        std::string name = get_first_word(remaining);
        remaining = get_tail(remaining);
        std::string property = get_first_word(remaining);
        std::string value = get_tail(remaining);
        if (property == "") {
            std::cerr << "Could not parse EDIT command. "
                      << "Usage:\n    EDIT name property value\n";
        }
        else {
            f.edit(name, property, value);
        }
    }
    // View a property
    else if (to_upper(get_first_word(line)) == "VIEW") {
        std::string remaining = get_tail(line);
        std::string name = get_first_word(remaining);
        remaining = get_tail(remaining);
        std::string property = get_first_word(remaining);
        if (get_tail(remaining) != "") {
            std::cerr << "Could not parse VIEW command. "
                      << "Usage:\n    VIEW name [property]\n";
        }
        else if (property == "") {
            f.view_properties(name);
        }
        else {
            f.view(name, property);
        }
    }
    else if (to_upper(get_first_word(line)) == "LIST") {
        std::string remaining = get_tail(line);
        std::vector<std::string> properties;
        while (remaining != "") {
            std::string next = get_first_word(remaining);
            if (next != "") {
                properties.push_back(next);
            }
            remaining = get_tail(remaining);
        }
        f.list(properties);
    }
    else if (get_first_word(line) != "") {
        std::cerr << "Command not recognized.\n";
    }
    else if (line.size() > 0 && line[0] == ' ') {
        do_command(line.substr(1, line.length() - 1), prompt);
        prompt = false;
    }
    if (prompt) {
        std::cout << ">>> ";
    }
    return true;
}
