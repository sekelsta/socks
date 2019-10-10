#include "doc_table.hh"
#include "file_utils.hh"

using json = nlohmann::json;

void DocTable::seek_to_header_start(jsoninfo &doc) {
    long int loc = DOC_TABLE_START + doc.index * get_entry_size();
#ifdef DEBUG_FILE_IO
    std::cout << "Seeking to " << loc << " for document " << doc.index << ".\n";
#endif
    if (fseek(file, loc, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
}

jsoninfo *DocTable::find_by_name(std::string name) {
    for (std::vector<jsoninfo>::iterator it = documents.begin(); it != documents.end(); ++it) {
        if (it -> name == name) {
            // Convert iterator to pointer
            return &(*it);
        }
    }
    return nullptr;
}

void DocTable::set_file(FILE *newfile) {
    file = newfile;
}

int DocTable::get_entry_size() {
    return 2 * sizeof(int64_t);
}

std::vector<jsoninfo> DocTable::get_documents() {
    return documents;
}

// Adds a document to the table, and modifies the one passed to have the 
// correct values
// Returns 0 if successful or -1 if there was not enough room
int DocTable::add(jsoninfo &doc, Fileheader &header) {
    // First try to replace a deleted file
    for(unsigned int i = 0; i < documents.size(); ++i) {
        if (documents[i].is_deleted()) {
#ifdef DEBUG_FILE_IO
            std::cout << "Replacing deleted document " << i << "\n";
#endif
            doc.index = documents[i].index;
            doc.doc_start = documents[i].doc_start;
            documents[i].name = doc.name;
            documents[i].j = doc.j;

            jsoninfo change = documents[i];
            change.set_undeleted();
            modify(i, change);
            doc.allocated_size = documents[i].allocated_size;
            return 0;
        }
    }
    // Otherwise, add to the end
    // Check for room
    if (end + get_entry_size() > header.header_len) {
        return -1;
    }
    // Set new table end
    doc.index = documents.size();
    doc.doc_start = get_file_end();
    doc.allocated_size = BLOCK_SIZE;
    write(doc);
    documents.push_back(doc);
    header.set_num_docs(header.num_docs + 1, file);
    return 0;
}

void DocTable::write(jsoninfo &doc) {
    // Seek
    seek_to_header_start(doc);
#ifdef DEBUG_FILE_IO
    std::cout << "Writing doc_start as " << doc.doc_start << "\n";
#endif
    // Write document's location
    if (fwrite(&(doc.doc_start), sizeof(int64_t), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
    // Write allocated size
    if (fwrite(&(doc.allocated_size), sizeof(int64_t), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
}

void DocTable::remove(std::string name) {
    jsoninfo *doc = find_by_name(name);
    if (doc == nullptr) {
        std::cerr << "Document " << name << " not found.";
    }
    // Seek
    if (fseek(file, DOC_TABLE_START, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }

    doc->name = "";
    free(doc->j);
    doc->j = nullptr;
    jsoninfo newdoc = *doc;
    newdoc.set_deleted();
    modify(doc->index, newdoc);
}

void DocTable::modify(std::string name, jsoninfo doc) {
    jsoninfo *current = find_by_name(name);
    modify(current->index, doc);
}
void DocTable::modify(int index, jsoninfo doc) {
    doc.index = index;
    documents[index] = doc;
    write(doc);
}

int DocTable::get_doc_start(int index) {
    return documents[index].doc_start;
}

int DocTable::get_doc_start(std::string name) {
    return get_doc_start(find_by_name(name) -> index);
}

bool DocTable::is_name_used(std::string name) {
    return find_by_name(name) != nullptr;
}

void DocTable::read_json(jsoninfo *js) {
    if (js -> j != nullptr) {
        std::cout << "Warning: Destination json already exists.\n";
        delete js -> j;
    }
#ifdef DEBUG_FILE_IO
    std::cout << "Reading from " << js -> doc_start << "\n";
#endif
    fseek(file, js -> doc_start, SEEK_SET);
    // Read the size of the json file
    int len;
    if (fread(&len, sizeof(int), 1, file) != 1) {
        perror(FILE_IO_ERROR);
    }
    // Read the file
    char *jdata = (char *)malloc(len);
    if (fgets(jdata, len, file) == NULL) {
        perror(FILE_IO_ERROR);
    }
    // Convert to string to json
    std::string s(jdata);
    (js -> j) = new json;
    *(js -> j) = json::parse(s);
}


void DocTable::read(int num_docs) {
    // Seek
    if (fseek(file, DOC_TABLE_START, SEEK_SET) != 0) {
        perror(FILE_IO_ERROR);
    }
    // Read
    for (int i = 0; i < num_docs; ++i) {
#ifdef DEBUG_FILE_IO
        std::cout << "Reading header from " << ftell(file) << "\n";
#endif
        // Read int for file position
        long int pos;
        if (fread(&pos, sizeof(int64_t), 1, file) != 1) {
            perror(FILE_IO_ERROR);
        }
#ifdef DEBUG_FILE_IO
        std::cout << "Position read from header: " << pos << "\n";
#endif
        // Read int for allocated size
        long int size;
        if (fread(&size, sizeof(int64_t), 1, file) != 1) {
            perror(FILE_IO_ERROR);
        }
        if (feof(file) != 0) {
            std::cerr << UNEXPECTED_EOF_ERROR;
        }
        documents.push_back({nullptr, pos, i, size, ""});
    }
    end = ftell(file);
    if (end == -1) {
            perror(FILE_IO_ERROR);
    }
    assert(end == DOC_TABLE_START + (num_docs * get_entry_size()));
    // Read jsons
    for (int i = 0; i < num_docs; ++i) {
        if (!documents[i].is_deleted()) {
            read_json(&(documents[i]));
            documents[i].name = (*(documents[i].j))["__name"];
        }
    }
}

json *DocTable::get_json(std::string name) {
    return find_by_name(name) -> j;
}

int DocTable::get_file_end() {
    if (documents.size() == 0) {
        return DEFAULT_HEADER_LEN;
    }
    return documents.back().doc_start + documents.back().allocated_size;
}

void DocTable::shift(int start, int new_start) {
    for(unsigned int i = 0; i < documents.size(); ++i) {
        if (documents[i].doc_start >= start) {
            jsoninfo change = documents[i];
            change.doc_start += new_start - start;
            modify(i, change);
        }
    }
}

void DocTable::free_jsons() {
    for(unsigned int i = 0; i < documents.size(); ++i) {
        if (documents[i].j != nullptr) {
            delete documents[i].j;
            documents[i].j = nullptr;
        }
    }
}

void DocTable::close() {
    free_jsons();
    documents.clear();
    file = nullptr;
}
