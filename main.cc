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

std::string get_first_word(std::string s) {
    unsigned int n = s.find(" ");
    if (n == std::string::npos) {
        return s;
    }
    return s.substr(0, n);
}

std::string get_tail(std::string s) {
    unsigned int n = s.find(" ");
    if (n == std::string::npos || n == s.length() - 1) {
        return "";
    }
    return s.substr(n + 1, s.length());
}

int main(int argc, char *argv[]) {
    std::cout << "Welcome to this db. Type QUIT to exit.\n>>> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "QUIT") {
            break;
        }
        else if (get_first_word(line) == "CREATE") {
            if (get_tail(line).find(" ") == std::string::npos) {
                create(get_tail(line));
            }
            else {
                std::cerr << "Error: Name cannot contain a space:\n    \""
                          << get_tail(line) << "\"\n";
            }
        }
        else {
            std::cerr << "Command not recognized.\n";
        }
        std::cout << ">>> ";
    }
}

void create(std::string name) {
    json *j = new json;
    documents.push_back(j);
    // TODO: actually pick a reasonable id
    (*j)["__id"] = 0;
    (*j)["__name"] = name;

    std::ofstream outfile(name);
    if (!outfile) {
        std::cerr << "Can't open " << name << "\n";
    }
    // setw for making more readable and not  really needed
    outfile << std::setw(4) << *j << std::endl;
    outfile.close();
}
