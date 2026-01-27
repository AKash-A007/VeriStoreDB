#include "db/database.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace vsdb {

Database::Database() 
    : db_root_(std::filesystem::current_path()) {
}

bool Database::is_initialized() const {
    return std::filesystem::exists(db_root_ / ".vsdb");
}

std::filesystem::path Database::get_db_path() const {
    return db_root_;
}

bool Database::initialize() {
    if (is_initialized()) {
        std::cerr << "Error: Database already initialized in this directory.\n";
        return false;
    }
    
    if (!create_directory_structure()) {
        std::cerr << "Error: Failed to create directory structure.\n";
        return false;
    }
    
    if (!create_config_file()) {
        std::cerr << "Error: Failed to create configuration file.\n";
        return false;
    }
    
    std::cout << "Database initialized successfully in " << db_root_ << "\n";
    std::cout << "Created directories:\n";
    std::cout << "  - data/      (for table data storage)\n";
    std::cout << "  - objects/   (for version control objects)\n";
    
    return true;
}

bool Database::create_directory_structure() {
    try {
        std::filesystem::create_directories(db_root_ / "data");
        std::filesystem::create_directories(db_root_ / "objects");
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
        return false;
    }
}

bool Database::create_config_file() {
    try {
        std::ofstream config_file(db_root_ / ".vsdb");
        
        if (!config_file.is_open()) {
            return false;
        }
        
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        
        config_file << "version=1.0\n";
        config_file << "initialized=" << ss.str() << "\n";
        config_file << "format=vsdb\n";
        
        config_file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating config file: " << e.what() << "\n";
        return false;
    }
}

} // namespace vsdb