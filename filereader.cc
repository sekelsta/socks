#include "filereader.hh"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using json = nlohmann::json;

int Filereader::get_doc_start(std::string name) {
    int start = find_by_name(name) -> doc_start;
    if (start == INVALID_LOCATION) {
        std::cerr << "Start of document " << name << " not set.\n";
    }
    return start;
}

void Filereader::parse_fail() {
    std::cerr << "Could not parse file.\n";
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

void Filereader::read_json(jsoninfo *js) {
    if (js -> j != nullptr) {
        std::cout << "Warning: Destination json already exists.\n";
        delete js -> j;
    }
    fseek(file, js -> doc_start, SEEK_SET);
    // Read the size of the json file
    int len;
    if (fread(&len, sizeof(int), 1, file) != 1) {
        parse_fail();
    }
    // Read the file
    char *jdata = (char *)malloc(len);
    if (fgets(jdata, len, file) == NULL) {
        parse_fail();
    }
    // Convert to string to json
    std::string s(jdata);
    js -> j = new json;
    *(js -> j) = json::parse(s);
}

void Filereader::write(jsoninfo *js) {
    std::stringstream output;
    output << *(js -> j) << std::endl;
    int len = output.str().length();
    if (len + sizeof(int) >= js -> allocated_size) {
        extend(js, len);
    }
    fseek(file, get_doc_start((*(js -> j))["__name"]), SEEK_SET);
    fwrite(&len, sizeof(int), 1, file);
    fputs(output.str().c_str(), file);
}

// Find the first document with this name.
jsoninfo *Filereader::find_by_name(std::string name) {
    for (std::vector<jsoninfo>::iterator it = documents.begin(); it != documents.end(); ++it) {
        if (it -> name == name) {
            // Convert iterator to pointer
            return &(*it);
        }
    }
    return nullptr;
}

void Filereader::create(std::string name) {
    // Require name to be unique
    if (find_by_name(name) != nullptr) {
        std::cerr << "A document with that name already exists.\n";
        return;
    }
    // Add header
    int header_size = 2 * sizeof(int) + name.length();
    // Default initial size
    int header_end = header_size;
    if (header.num_docs > 0) {
        // Find the end of the last header
        int header_end = documents.back().header_start + sizeof(int) 
            + sizeof(char) + documents.back().name.length();
        fseek(file, header_end, SEEK_SET);
        
        // check size
        if (header_size + header_end >= header.header_len) {
            extend_header(header_size + header_end);
            create(name);
            return;
        }
    }
    // write new header
    fseek(file, header_end, SEEK_SET);
    // Get position of header start
    long int header_pos = ftell(file);
    // Write int for file position
    fwrite(&end, sizeof(int), 1, file);
    // Write char for name length
    fputc((char)name.length(), file); // 1 (x HEADER_LEN)
    // Write name
    fputs(name.c_str(), file);
    // Add document to list
    json *j = new json;
    documents.push_back({j, end, header_pos, BLOCK_SIZE, name});
    (*j)["__name"] = name;
    // Write
    header.set_num_docs(header.num_docs + 1, file);
    end += BLOCK_SIZE;
    write(&documents.back());
}

void Filereader::edit(std::string name, std::string property, std::string value) {
    // Find the json with that name
    jsoninfo *js = find_by_name(name);
    if (!js) {
        std::cerr << "Name not recognized: " << name << "\n";
    }
    else {
        try {
            (*(js -> j))[property] = json::parse(value);
        }
        catch (nlohmann::detail::parse_error &e) {
            std::cerr << "Could not parse:\n    " << value << "\nError:\n" 
                      << e.what() << "\n";
        }
        write(js);
    }
}

void Filereader::view(std::string name, std::string property) {
    // Find the json with that name
    json *j = find_by_name(name) -> j;
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
        header.write_header(file);
        return;
    }
    // Read headers
    for (int i = 0; i < header.num_docs; ++i) {
        // Get position of header start
        long int header_pos = ftell(file);
        // Read int for file position
        int pos;
        if (fread(&pos, sizeof(int), 1, file) != 1) {
            parse_fail();
        }
        // Read char for name length
        int name_len = (int)fgetc(file);
        // Read name
        char *name = (char *)malloc(name_len);
        if (fgets(name, name_len, file) == NULL) {
            parse_fail();
        }
        documents.push_back({nullptr, pos, header_pos, 0, std::string(name)});
        free(name);
    }
    // Set allocated size properly
    for (int i = 0; i < header.num_docs - 1; ++i) {
        documents[i].allocated_size = documents[i+1].doc_start - documents[i].doc_start;
    }
    // TODO: don't read until needed
    // Read jsons
    for (int i = 0; i < header.num_docs; ++i) {
        read_json(&(documents[i]));
    }
    if (header.num_docs > 0) {
        int last_len = documents.back().j->dump().length();
        // Round up to a multiple of BLOCK_SIZE
        documents.back().allocated_size = BLOCK_SIZE * ceil(last_len/BLOCK_SIZE);
    }
}
