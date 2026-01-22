#include "cli/cli_parser.h"
#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc ,char **argv) {
    CLI::App VeriStoreDB{"vsdb"} ;
    
    
    
    //init command 
    auto* init = VeriStoreDB.add_subcommand("init","initialize a new VSDB database ");

    //table insertion command
    auto* insert_cmd = VeriStoreDB.add_subcommand("insert", "insert into the table");
    std::string insert_table ;
    std::vector<std::string> insert_cols;
    std::vector<std::string> insert_vals;
    insert_cmd->add_option("table",insert_table,"Table name")->required();
    insert_cmd->add_option("--coloumns,-c",insert_cols,"Coloumn names"  );
    insert_cmd->add_option("--values,-v",insert_vals,"values");

    //select command
    auto* select_cmd = VeriStoreDB.add_subcommand("select","Select data from a table");
    std::string select_table;
    select_cmd->add_option("table",select_table,"Table name")->required();

    //commit command
    /*DO A DEFAULT COMMIT MSG in if condition */
    auto* commit_cmd = VeriStoreDB.add_subcommand("commit","Commit current changes");
    std::string commit_msg;
    commit_cmd->add_option("-m, --message",commit_msg,"Commit message")->required()
    
    //LOG command
    auto* log_cmd = VeriStoreDB.add_subcommand("log","Show commit history");

    //CHECKout command
    auto* checkout_cmd = VeriStoreDB.add_subcommand("checkout","Checkout a specific commit");
    std::string checkout_hash;
    checkout_cmd->add_option("commit",checkout_hash,"Commit hash")->required();
    
    CLI11_PARSE(VeriStoreDB, argc,argv)
    
    if (VeriStoreDB.got_subcommand(init)) {std::cout << "initillizedaaa\n";};
    return 0;
}