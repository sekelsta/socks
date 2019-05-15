#include "filereader.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using json = nlohmann::json;

void Filereader::write(json *j) {
    // If I wanted to append
    // std::ofstream outfile((*j)["__name"], std::ios::out | std::ios::app);
    // TODO: allocate space for each document like a vector
    std::ofstream outfile((*j)["__name"]);
    if (!outfile) {
        std::cerr << "Can't open " << (*j)["__name"] << "\n";
    }
    // setw for making more readable and not  really needed
    outfile << std::setw(4) << *j << std::endl;
    outfile.close();
}

// Find the first document with this name.
json *Filereader::find_by_name(std::string name) {
    for (std::vector<json*>::iterator it = documents.begin(); it != documents.end(); ++it) {
        if ((**it)["__name"] == name) {
            return *it;
        }
    }
    return nullptr;
}

void Filereader::create(std::string name) {
    json *j = new json;
    documents.push_back(j);
    // TODO: Require name to be unique
    (*j)["__name"] = name;

    write(j);
}

void Filereader::edit(std::string name, std::string property, std::string value) {
    // Find the json with that name
    json *j = find_by_name(name);
    if (!j) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else {
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

void Filereader::view(std::string name, std::string property) {
    // Find the json with that name
    json *j = find_by_name(name);
    if (!j) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else {
        std::cout << (*j)[property] << "\n";
    }
}

void Filereader::del(std::string name) {
    std::cerr << "DELETE not implemented.\n";
}

void Filereader::open(std::string name) {
    // mode r+, read and write
    file = fopen(name.c_str(), "r+");
    if (file == NULL) {
        // Create file if needed
        file = fopen(name.c_str(), "w");
        if (file == NULL) {
            std::cerr << "Error: unable to open file:\n    " << name << "\n";
            return;
        }
        fputc(0, file);
    }
}
