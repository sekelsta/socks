#include "filereader.hh"
#include "file_utils.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using json = nlohmann::json;

void Filereader::parse_fail() {
    std::cerr << "Error: Could not parse file.\n";
    fclose(file);
    file = NULL;
    header.num_docs = 0;
    header.header_len = DEFAULT_HEADER_LEN;
}

void Filereader::extend_header() {
    if (header.num_docs > 0) {
        file_shift(header.header_len, 2 * header.header_len, file);
        table.shift(header.header_len, 2 * header.header_len, file);
    }
    header.set_header_len(2 * header.header_len, file);
}

void Filereader::extend(jsoninfo *js, int size_needed) {
    int new_size = js->allocated_size;
    while (new_size <= size_needed) {
        new_size *= 2;
    }
    if (js->name != table.documents.back().name) {
        int start = js->doc_start + js->allocated_size;
        int new_start = js->doc_start + new_size;
        file_shift(start, new_start, file);
        table.shift(start, new_start, file);
    }
    jsoninfo modified = *js;
    modified.allocated_size = new_size;
    table.modify(modified.name, modified, file);
}

void Filereader::write(jsoninfo *js) {
    std::stringstream output;
    output << *(js -> j) << std::endl;
    int len = output.str().length();
    if (len + sizeof(int) >= js -> allocated_size) {
        extend(js, len);
    }
    fseek(file, table.get_doc_start((*(js -> j))["__name"], file), SEEK_SET);
    fwrite(&len, sizeof(int), 1, file);
    fputs(output.str().c_str(), file);
}

void Filereader::create(std::string name) {
    // Require name to be unique
    if (table.is_name_used(name, file)) {
        std::cerr << "Error: A document with that name already exists.\n";
        return;
    }
    // Name length is stored as a char
    if (name.size() > 255) {
        std::cerr << "Error: Names can be at most 255 characters long.\n"
                  << "\"" << name << "\" is " << name.size()
                  << " characters long.\n";
        return;
    }
    // Gather document info
    json *j = new json;
    jsoninfo js = {j, table.get_file_end(), INVALID_LOCATION, BLOCK_SIZE, name};
    // If needed, double the header length until the new entry fits
    while (table.add(js, header.header_len, file)) {
        extend_header();
    }
    table.modify(name, js, file);
    // TODO: move this into jsoninfo constructor
    (*j)["__name"] = name;
    // Write
    header.set_num_docs(header.num_docs + 1, file);
    write(&js);
}

// Returns true if the property does not start with "__"
bool Filereader::is_property_hidden(std::string property) {
    return (property.substr(0, 2) == "__");
}

void Filereader::edit(std::string name, std::string property, std::string value) {
    // Check if the document exists
    if (!table.is_name_used(name, file)) {
        std::cerr << "Error: Name not recognized: " << name << "\n";
        return;
    }
    // Find the json with that name
    jsoninfo *js = table.find_by_name(name, file);
    if (!js) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else if (is_property_hidden(property)) {
        std::cerr << "Cannot edit internal (hidden) property \"" 
                  << property << "\".\n";
    }
    else {
        try {
            (*(js -> j))[property] = json::parse(value);
        }
        catch (nlohmann::detail::parse_error &e) {
            std::cerr << "Error: Could not parse:\n    " << value << "\nError:\n" 
                      << e.what() << "\n";
        }
        write(js);
    }
}

void Filereader::view(std::string name, std::string property) {
    // Check if the document exists
    if (!table.is_name_used(name, file)) {
        std::cerr << "Error: Name not recognized: " << name << "\n";
        return;
    }
    // Find the json with that name
    json *j = table.get_json(name, file);
    assert(j != nullptr);
    if (!(*j).count(property)) {
        std::cerr << "Error: \"" << name << "\" does not have \"" 
                  << property << "\".\n";
    }
    else {
        if (is_property_hidden(property)) {
            std::cout << "Viewing hidden property \"" << property << "\":\n";
        }
        std::cout << (*j)[property] << "\n";
    }
}

void Filereader::view_properties(std::string name) {
    // Check if the document exists
    if (!table.is_name_used(name, file)) {
        std::cerr << "Error: Name not recognized: " << name << "\n";
        return;
    }
    // Find the json with that name
    json *j = table.get_json(name, file);
    assert(j != nullptr);
    for (json::iterator it = (*j).begin(); it != (*j).end(); ++it) {
        if (!is_property_hidden(it.key())) {
            std::cout << it.key() << ": " << it.value() << "\n";
        }
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
        file = fopen(name.c_str(), "w+");
        if (file == NULL) {
            std::cerr << "Error: unable to open file:\n    " << name << "\n";
            return;
        }
        header.write_header(file);
        return;
    }
    // Read headers
    header.read_header(file);
    table.read(header.num_docs, file);
}

void Filereader::list() {
    table.list(file);
}
