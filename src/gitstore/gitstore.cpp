#include "gitstore/gitstore.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>

namespace vsdb {

GitStore::GitStore(const std::filesystem::path& objects_dir) 
    : objects_dir_(objects_dir), head_file_(objects_dir.parent_path() / ".vsdb_head") {
    std::filesystem::create_directories(objects_dir_);
}

std::string GitStore::generate_hash(const std::string& content) {
    // Simple hash function (not cryptographic)
    // In production, use SHA-1 or SHA-256
    std::hash<std::string> hasher;
    size_t hash_value = hasher(content);
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash_value;
    return ss.str();
}

std::string GitStore::hash_file(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return generate_hash(buffer.str());
}

std::string GitStore::store_file(const std::filesystem::path& file_path) {
    std::ifstream src(file_path, std::ios::binary);
    if (!src.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << src.rdbuf();
    std::string content = buffer.str();
    
    std::string hash = generate_hash(content);
    std::filesystem::path obj_path = objects_dir_ / hash;
    
    // Don't store if already exists
    if (std::filesystem::exists(obj_path)) {
        return hash;
    }
    
    std::ofstream dst(obj_path, std::ios::binary);
    dst << content;
    
    return hash;
}

bool GitStore::restore_file(const std::string& hash, const std::filesystem::path& target_path) {
    std::filesystem::path obj_path = objects_dir_ / hash;
    
    if (!std::filesystem::exists(obj_path)) {
        std::cerr << "Error: Object " << hash << " not found\n";
        return false;
    }
    
    std::ifstream src(obj_path, std::ios::binary);
    std::ofstream dst(target_path, std::ios::binary);
    
    dst << src.rdbuf();
    return true;
}

std::string Commit::serialize() const {
    std::stringstream ss;
    ss << "hash=" << hash << "\n";
    ss << "message=" << message << "\n";
    ss << "timestamp=" << timestamp << "\n";
    ss << "parent=" << parent_hash << "\n";
    ss << "files=" << file_hashes.size() << "\n";
    
    for (const auto& fh : file_hashes) {
        ss << fh << "\n";
    }
    
    return ss.str();
}

Commit Commit::deserialize(const std::string& data) {
    Commit commit;
    std::istringstream ss(data);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            commit.file_hashes.push_back(line);
            continue;
        }
        
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        if (key == "hash") {
            commit.hash = value;
        } else if (key == "message") {
            commit.message = value;
        } else if (key == "timestamp") {
            commit.timestamp = value;
        } else if (key == "parent") {
            commit.parent_hash = value;
        } else if (key == "files") {
            // Next lines are file hashes
        }
    }
    
    return commit;
}

bool GitStore::save_commit(const Commit& commit) {
    std::filesystem::path commit_path = objects_dir_ / commit.hash;
    std::ofstream file(commit_path);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << commit.serialize();
    return true;
}

std::optional<Commit> GitStore::load_commit(const std::string& hash) const {
    std::filesystem::path commit_path = objects_dir_ / hash;
    
    if (!std::filesystem::exists(commit_path)) {
        return std::nullopt;
    }
    
    std::ifstream file(commit_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return Commit::deserialize(buffer.str());
}

bool GitStore::update_head(const std::string& commit_hash) {
    std::ofstream file(head_file_);
    if (!file.is_open()) {
        return false;
    }
    
    file << commit_hash;
    return true;
}

std::optional<std::string> GitStore::get_head() const {
    if (!std::filesystem::exists(head_file_)) {
        return std::nullopt;
    }
    
    std::ifstream file(head_file_);
    std::string hash;
    std::getline(file, hash);
    
    return hash.empty() ? std::nullopt : std::make_optional(hash);
}

std::string GitStore::commit(const std::string& message, const std::filesystem::path& data_dir) {
    Commit new_commit;
    new_commit.message = message;
    
    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ts;
    ts << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    new_commit.timestamp = ts.str();
    
    // Get parent commit
    auto head = get_head();
    new_commit.parent_hash = head.value_or("");
    
    // Store all data files
    for (const auto& entry : std::filesystem::directory_iterator(data_dir)) {
        if (entry.is_regular_file()) {
            std::string hash = store_file(entry.path());
            if (!hash.empty()) {
                new_commit.file_hashes.push_back(entry.path().filename().string() + ":" + hash);
            }
        }
    }
    
    // Generate commit hash
    std::string commit_content = new_commit.message + new_commit.timestamp + new_commit.parent_hash;
    for (const auto& fh : new_commit.file_hashes) {
        commit_content += fh;
    }
    new_commit.hash = generate_hash(commit_content);
    
    // Save commit and update HEAD
    if (!save_commit(new_commit)) {
        return "";
    }
    
    update_head(new_commit.hash);
    
    return new_commit.hash;
}

std::vector<Commit> GitStore::get_log() const {
    std::vector<Commit> log;
    
    auto head = get_head();
    if (!head) {
        return log;
    }
    
    std::string current_hash = *head;
    
    while (!current_hash.empty()) {
        auto commit = load_commit(current_hash);
        if (!commit) {
            break;
        }
        
        log.push_back(*commit);
        current_hash = commit->parent_hash;
    }
    
    return log;
}

bool GitStore::checkout(const std::string& commit_hash, const std::filesystem::path& data_dir) {
    auto commit = load_commit(commit_hash);
    if (!commit) {
        std::cerr << "Error: Commit " << commit_hash << " not found\n";
        return false;
    }
    
    // Clear data directory
    for (const auto& entry : std::filesystem::directory_iterator(data_dir)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry.path());
        }
    }
    
    // Restore all files from commit
    for (const auto& file_hash : commit->file_hashes) {
        size_t colon_pos = file_hash.find(':');
        if (colon_pos == std::string::npos) continue;
        
        std::string filename = file_hash.substr(0, colon_pos);
        std::string hash = file_hash.substr(colon_pos + 1);
        
        if (!restore_file(hash, data_dir / filename)) {
            std::cerr << "Warning: Failed to restore " << filename << "\n";
        }
    }
    
    // Update HEAD
    update_head(commit_hash);
    
    return true;
}

} // namespace vsdb