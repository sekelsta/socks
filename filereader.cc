#include "filereader.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using json = nlohmann::json;

int Filereader::get_doc_start(std::string name) {
    // TODO
    return HEADER_LEN;
}

void Filereader::write(json *j) {
    std::stringstream output;
    output << *j << std::endl;
    int len = output.str().length();
    fseek(file, get_doc_start((*j)["__name"]), SEEK_SET);
    fwrite(&len, sizeof(int), 1, file);
    fputs(output.str().c_str(), file);
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
        num_docs = 0;
        fputc(header_len, file); // 1 (x HEADER_LEN)
        fputc(num_docs, file); // 0
        return;
    }
    header_len = (int)fgetc(file);
    num_docs = (int)fgetc(file);
}
