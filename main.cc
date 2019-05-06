#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

// Global for now
std::vector<json*> documents;

void create(std::string name);

void edit(std::string name, std::string property, std::string value);

void view(std::string name, std::string property);

void del(std::string name);

std::string get_first_word(std::string s) {
    size_t n = s.find(" ");
    if (n == std::string::npos) {
        return s;
    }
    return s.substr(0, n);
}

std::string get_tail(std::string s) {
    // TODO: deal with unexpected whitespace
    size_t n = s.find(" ");
    if (n == std::string::npos || n == s.length() - 1) {
        return "";
    }
    return s.substr(n + 1, s.length());
}

// Returns true if the property does not start with "__"
bool check_property_valid(std::string property) {
    return (property.substr(0, 2) != "__");
}

void invalid_property_message(std::string property) {
    std::cerr << "Error: Property names beginning in \"__\" are not "
              << "allowed:\n    " << property;
}

int main(int argc, char *argv[]) {
    std::cout << "Welcome to this db. Type QUIT to exit.\n>>> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        // Quit
        if (line == "QUIT") {
            break;
        }
        // Create a document
        else if (get_first_word(line) == "CREATE") {
            if (get_tail(line) != "" 
                    && get_tail(line).find(" ") == std::string::npos) {
                create(get_tail(line));
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
                del(get_tail(line));
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
                edit(name, property, value);
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
                view(name, property);
            }
        }
        else {
            std::cerr << "Command not recognized.\n";
        }
        std::cout << ">>> ";
    }
}

void write(json *j) {
    // If I wanted to append
    // std::ofstream outfile((*j)["__name"], std::ios::out | std::ios::app);
    std::ofstream outfile((*j)["__name"]);
    if (!outfile) {
        std::cerr << "Can't open " << (*j)["__name"] << "\n";
    }
    // setw for making more readable and not  really needed
    outfile << std::setw(4) << *j << std::endl;
    outfile.close();
    // TODO: 
}

// Find the first document with this name.
json *find_by_name(std::string name) {
    for (std::vector<json*>::iterator it = documents.begin(); it != documents.end(); ++it) {
        if ((**it)["__name"] == name) {
            return *it;
        }
    }
    return nullptr;
}

void create(std::string name) {
    json *j = new json;
    documents.push_back(j);
    // TODO: Require name to be unique
    (*j)["__name"] = name;

    write(j);
}

void edit(std::string name, std::string property, std::string value) {
    // Find the json with that name
    json *j = find_by_name(name);
    if (!j) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else {
        // TODO: error handling
        try {
            (*j)[property] = json::parse(value);
        }
        catch (nlohmann::detail::parse_error &e) {
            std::cerr << "Could not parse:\n    " << value << "\nError:\n" 
                      << e.what() << "\n";
        }
        write(j);
    }
}

void view(std::string name, std::string property) {
    // Find the json with that name
    json *j = find_by_name(name);
    if (!j) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else {
        std::cout << (*j)[property] << "\n";
    }
}

void del(std::string name) {
    std::cerr << "DELETE not implemented.\n";
}
