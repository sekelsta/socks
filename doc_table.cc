#include "doc_table.hh"

using json = nlohmann::json;

jsoninfo *DocTable::find_by_name(std::string name, FILE *file) {
    for (std::vector<jsoninfo>::iterator it = documents.begin(); it != documents.end(); ++it) {
        if (it -> name == name) {
            // Convert iterator to pointer
            return &(*it);
        }
    }
    return nullptr;
}


int DocTable::add(jsoninfo &doc, int header_len, FILE *file) {
    // Check for room
    int length = sizeof(int) + sizeof(char) + doc.name.length();
    if (end + length > header_len) {
        return -1;
    }
    // Set new table end
    doc.header_start = end;
    write(doc, file);
    end = ftell(file);
    if (end == -1L) {
        throw "TODO: deal with file errors";
    }
    documents.push_back(doc);
    return 0;
}

void DocTable::write(jsoninfo &doc, FILE *file) {
    // Seek
    if (fseek(file, doc.header_start, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    // Write document's location
    if (fwrite(&(doc.doc_start), sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
    // Write name length
    if (fputc((char)doc.name.length(), file) == EOF) {
        throw "TODO: deal with file errors";
    }
    // Write name
    if (fputs(doc.name.c_str(), file) == EOF) {
        throw "TODO: deal with file errors";
    }
}

void DocTable::remove(std::string name, FILE *file) {
    jsoninfo *doc = find_by_name(name, file);
    if (doc == nullptr) {
        std::cerr << "Document " << name << " not found.";
    }
    // Seek
    if (fseek(file, DOC_TABLE_START, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }

    // TODO
    throw "TODO: remove documents";
}

void DocTable::modify(std::string name, jsoninfo doc, FILE *file) {
    jsoninfo *current = find_by_name(name, file);
    if (current == nullptr) {
        std::cerr << "Document " << name << " not found.";
    }
    // Seek
    if (fseek(file, DOC_TABLE_START, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    
    // TODO
    if (name.length() != doc.name.length()) {
        throw "TODO: document name length change";
    }
    else {
        doc.header_start = current->header_start;
        *current = doc;
        write(doc, file);
    }
}

int DocTable::get_doc_start(std::string name, FILE *file) {
    int start = find_by_name(name, file) -> doc_start;
    if (start == INVALID_LOCATION) {
        std::cerr << "Start of document " << name << " not set.\n";
    }
    return start;
}

bool DocTable::is_name_used(std::string name, FILE *file) {
    return find_by_name(name, file) != nullptr;
}

void DocTable::read_json(jsoninfo *js, FILE *file) {
    if (js -> j != nullptr) {
        std::cout << "Warning: Destination json already exists.\n";
        delete js -> j;
    }
    fseek(file, js -> doc_start, SEEK_SET);
    // Read the size of the json file
    int len;
    if (fread(&len, sizeof(int), 1, file) != 1) {
        throw "TODO: deal with file errors";
    }
    // Read the file
    char *jdata = (char *)malloc(len);
    if (fgets(jdata, len, file) == NULL) {
        throw "TODO: deal with file errors";
    }
    // Convert to string to json
    std::string s(jdata);
    (js -> j) = new json;
    *(js -> j) = json::parse(s);
}


void DocTable::read(int num_docs, FILE *file) {
    // Seek
    if (fseek(file, DOC_TABLE_START, SEEK_SET) != 0) {
        throw "TODO: deal with file errors";
    }
    // Read
    for (int i = 0; i < num_docs; ++i) {
        // Get position of header start
        long int header_pos = ftell(file);
        if (header_pos == -1) {
            throw "TODO: deal with file errors";
        }
        // Read int for file position
        int pos;
        if (fread(&pos, sizeof(int32_t), 1, file) != 1) {
            throw "TODO: deal with file errors";
        }
        // Read char for name length
        int name_len = fgetc(file);
        if (name_len == EOF) {
            throw "TODO: deal with file errors";
        }
        else if (name_len == 0) {
            throw "Error: invalid format (names cannot be length 0)";
        }
        // Read name (malloc enough space for the ending '\0')
        char *name = (char *)malloc(name_len + 1);
        if (fgets(name, name_len + 1, file) == NULL) {
            if (feof(file) != 0) {
                throw "TODO: deal with file errors (encountered unexpected EOF)";
            }
            else {
                std::cerr << "ferror code: " << ferror(file) << "\n";
                throw "TODO: deal with file errors";
            }
        }
        if (feof(file) != 0) {
            throw "TODO: deal with file errors (encountered unexpected EOF)";
        }
        documents.push_back({nullptr, pos, header_pos, 0, std::string(name)});
        free(name);
    }
    end = ftell(file);
    if (end == -1) {
            throw "TODO: deal with file errors";
    }

    // Set allocated size properly
    for (int i = 0; i < num_docs - 1; ++i) {
        documents[i].allocated_size = documents[i+1].doc_start - documents[i].doc_start;
    }
    // TODO: don't read until needed
    // Read jsons
    for (int i = 0; i < num_docs; ++i) {
        read_json(&(documents[i]), file);
    }
    if (num_docs > 0) {
        int last_len = documents.back().j->dump().length();
        // Round up to a multiple of BLOCK_SIZE
        documents.back().allocated_size = BLOCK_SIZE * ceil(last_len/BLOCK_SIZE);
    }
}

json *DocTable::get_json(std::string name, FILE *file) {
    return find_by_name(name, file) -> j;
}

int DocTable::get_file_end() {
    if (documents.size() == 0) {
        return DEFAULT_HEADER_LEN;
    }
    return documents.back().doc_start + documents.back().allocated_size;
}

void DocTable::shift(int start, int new_start, FILE *file) {
    for(unsigned int i = 0; i < documents.size(); ++i) {
        if (documents[i].doc_start >= start) {
            jsoninfo change = documents[i];
            change.doc_start += new_start - start;
            modify(documents[i].name, change, file);
        }
    }
}


void DocTable::list(FILE *file) {
    for(unsigned int i = 0; i < documents.size(); ++i) {
        std::cout << documents[i].name << "\n";
    }
}
