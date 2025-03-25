#include "memory_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Helper function to extract quoted string
std::string extractQuotedString(std::istringstream& iss) {
    std::string data;
    char ch;
    iss >> std::ws; // Skip whitespace
    iss >> ch; // Read first quote
    
    if (ch != '"') {
        // Not a quoted string, read back the character
        iss.unget();
        iss >> data;
        return data;
    }
    
    std::getline(iss, data, '"'); // Read until closing quote
    return data;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command_file.cmmd>" << std::endl;
        return 1;
    }

    initMemoryManager();

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        
        // Remove comments and trim
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line = trim(line);
        
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        std::cout << "\nProcessing line " << line_number << ": " << line << std::endl;

        try {
            if (command == "INSERT") {
                int size;
                std::string data;
                if (iss >> size) {
                    data = extractQuotedString(iss);
                    std::cout << "Inserting: '" << data << "' with size: " << size << std::endl;
                    int id = insert(size, data);
                    std::cout << "Allocated ID: " << id << std::endl;
                } else {
                    std::cerr << "Error: Missing size in INSERT command" << std::endl;
                }
            }
            else if (command == "READ") {
                int id;
                if (iss >> id) {
                    std::cout << "Reading ID: " << id << std::endl;
                    find(id);
                } else {
                    std::cerr << "Error: Missing ID in READ command" << std::endl;
                }
            }
            else if (command == "UPDATE") {
                int id;
                std::string newData;
                if (iss >> id) {
                    newData = extractQuotedString(iss);
                    std::cout << "Updating ID " << id << " with: '" << newData << "'" << std::endl;
                    update(id, newData);
                } else {
                    std::cerr << "Error: Missing ID in UPDATE command" << std::endl;
                }
            }
            else if (command == "DELETE") {
                int id;
                if (iss >> id) {
                    std::cout << "Deleting ID: " << id << std::endl;
                    deleteBlock(id);
                } else {
                    std::cerr << "Error: Missing ID in DELETE command" << std::endl;
                }
            }
            else if (command == "DUMP") {
                std::cout << "Dumping memory state:" << std::endl;
                dump();
            }
            else {
                std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception processing line " << line_number << ": " << e.what() << std::endl;
        }

        // Clear any error flags that might have been set
        iss.clear();
    }

    return 0;
}
