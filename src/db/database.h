#pragma once

#include <string>
#include <filesystem>

namespace vsdb {

class Database {
public:
    Database();
    
    // Initialize database structure
    bool initialize();
    
    // Check if database is already initialized
    bool is_initialized() const;
    
    // Get database root path
    std::filesystem::path get_db_path() const;
    
private:
    std::filesystem::path db_root_;
    
    bool create_directory_structure();
    bool create_config_file();
};

} // namespace vsdb