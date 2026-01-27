#include "cli/cli_parser.h"
#include "db/database.h"
#include <iostream>

int main(int argc, char** argv) {
    vsdb::CLIParser parser;
    vsdb::ParsedCommand cmd = parser.parse(argc, argv);
    
    vsdb::Database db;
    
    switch (cmd.cmd) {
        case vsdb::Command::INIT:
            if (db.initialize()) {
                return 0;
            } else {
                return 1;
            }
            
        case vsdb::Command::INSERT:
            if (!db.is_initialized()) {
                std::cerr << "Error: Database not initialized. Run 'vsdb init' first.\n";
                return 1;
            }
            std::cout << "INSERT into table '" << cmd.table_name << "' (not yet implemented)\n";
            break;
            
        case vsdb::Command::SELECT:
            if (!db.is_initialized()) {
                std::cerr << "Error: Database not initialized. Run 'vsdb init' first.\n";
                return 1;
            }
            std::cout << "SELECT from table '" << cmd.table_name << "' (not yet implemented)\n";
            break;
            
        case vsdb::Command::COMMIT:
            if (!db.is_initialized()) {
                std::cerr << "Error: Database not initialized. Run 'vsdb init' first.\n";
                return 1;
            }
            std::cout << "COMMIT with message: '" << cmd.commit_message << "' (not yet implemented)\n";
            break;
            
        case vsdb::Command::LOG:
            if (!db.is_initialized()) {
                std::cerr << "Error: Database not initialized. Run 'vsdb init' first.\n";
                return 1;
            }
            std::cout << "LOG (not yet implemented)\n";
            break;
            
        case vsdb::Command::CHECKOUT:
            if (!db.is_initialized()) {
                std::cerr << "Error: Database not initialized. Run 'vsdb init' first.\n";
                return 1;
            }
            std::cout << "CHECKOUT commit '" << cmd.commit_hash << "' (not yet implemented)\n";
            break;
            
        case vsdb::Command::NONE:
        default:
            std::cerr << "No valid command specified.\n";
            return 1;
    }
    
    return 0;
}