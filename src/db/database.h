#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace vsdb {

enum class DataType{
    INT,
    FLOAT,
    TEXT,
    BOOL
};
struct Column{
    std::string name;
    DataType type;
    bool primary_key = false;
};

struct TableSchema{
    std::string table_name;
    std::vector<Column> columns;
    bool save_to_file(const std::filesystem::path& path) const;
    static TableSchema load_from_file(const std::filesystem::path& path);
};

struct Record{
    std::vector<std::string> values;
};
//---here we can define a Table class to manage records and schema---
// class Table{
// public:

// };
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