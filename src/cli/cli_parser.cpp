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
    CLI11_PARSE(VeriStoreDB, argc,argv)
    if (VeriStoreDB.got_subcommand(init)) {std::cout << "initillizedaaa\n";};
    return 0;
}