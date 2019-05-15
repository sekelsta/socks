#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"
#include "utils.hh"
#include "filereader.hh"

#define HEADER_LEN 1024

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    Filereader f;
    std::cout << "Welcome to this db. Type QUIT to exit.\n>>> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        // Quit
        if (line == "QUIT") {
            break;
        }
        else if (get_first_word(line) == "OPEN") {
            if (is_valid_filename(get_tail(line))) {
                f.open(get_tail(line));
            }
            else {
                std::cerr << "Error: Must be a valid file name:\n    \""
                          << get_tail(line) << "\"\n";
            }
        }
        // Create a document
        else if (get_first_word(line) == "CREATE") {
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
        else if (get_first_word(line) == "DELETE") {
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
        else if (get_first_word(line) == "EDIT") {
            std::string remaining = get_tail(line);
            std::string name = get_first_word(remaining);
            remaining = get_tail(remaining);
            std::string property = get_first_word(remaining);
            std::string value = get_tail(remaining);
            if (property == "") {
                std::cerr << "Could not parse EDIT command. "
                          << "Usage:\n    EDIT name property value\n";
            }
            else if (!check_property_valid(property)) {
                invalid_property_message(property);
            }
            else {
                f.edit(name, property, value);
            }
        }
        // View a property
        else if (get_first_word(line) == "VIEW") {
            std::string remaining = get_tail(line);
            std::string name = get_first_word(remaining);
            remaining = get_tail(remaining);
            std::string property = get_first_word(remaining);
            if (property == "" || get_tail(remaining) != "") {
                std::cerr << "Could not parse EDIT command. "
                          << "Usage:\n    EDIT name property value\n";
            }
            else if (!check_property_valid(property)) {
                invalid_property_message(property);
            }
            else {
                f.view(name, property);
            }
        }
        else {
            std::cerr << "Command not recognized.\n";
        }
        std::cout << ">>> ";
    }
}
