#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ctime>
#include <optional>

namespace vsdb {

struct Commit {
    std::string hash;
    std::string message;
    std::string timestamp;
    std::string parent_hash;
    std::vector<std::string> file_hashes; // Hashes of all data files
    
    std::string serialize() const;
    static Commit deserialize(const std::string& data);
};

class GitStore {
public:
    GitStore(const std::filesystem::path& objects_dir);
    
    // Create a new commit with current database state
    std::string commit(const std::string& message, const std::filesystem::path& data_dir);
    
    // Get commit history
    std::vector<Commit> get_log() const;
    
    // Restore database to a specific commit
    bool checkout(const std::string& commit_hash, const std::filesystem::path& data_dir);
    
    // Get current HEAD commit
    std::optional<std::string> get_head() const;
    
private:
    std::filesystem::path objects_dir_;
    std::filesystem::path head_file_;
    
    // Hash a file's contents
    std::string hash_file(const std::filesystem::path& file_path);
    
    // Store a file in objects directory
    std::string store_file(const std::filesystem::path& file_path);
    
    // Retrieve a file from objects directory
    bool restore_file(const std::string& hash, const std::filesystem::path& target_path);
    
    // Save commit object
    bool save_commit(const Commit& commit);
    
    // Load commit object
    std::optional<Commit> load_commit(const std::string& hash) const;
    
    // Update HEAD reference
    bool update_head(const std::string& commit_hash);
    
    // Generate simple hash (not cryptographic, just for demo)
    std::string generate_hash(const std::string& content);
};

} // namespace vsdb