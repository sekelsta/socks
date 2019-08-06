#include "filereader.hh"
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

void Filereader::extend_header(int size_needed) {
    // TODO
}

void Filereader::extend(jsoninfo *js, int size_needed) {
    //fseek(file, js -> header_start, SEEK_SET);
    //TODO
    // Choose a new size
    // Update the header
    // Move anything after it
    // Update their headers, and the in-memory values
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
    // Gather document info
    json *j = new json;
    jsoninfo js = {j, INVALID_LOCATION, INVALID_LOCATION, BLOCK_SIZE, name};
    if (table.add(js, header.header_len, file)) {
        std::cerr << "TODO: extend header\n";
    }
    js.doc_start = table.get_file_end();
    table.modify(name, js, file);
    // TODO: move this into jsoninfo constructor
    (*j)["__name"] = name;
    // Write
    header.set_num_docs(header.num_docs + 1, file);
    write(&js);
}

bool Filereader::is_property_hidden(std::string property) {
    return property.length() >= 2 
            && property[0] == '_' && property[1] == '_';
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
            std::cout << "Viewing hidden property \"" << property << "\"\n";
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
        file = fopen(name.c_str(), "w");
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
