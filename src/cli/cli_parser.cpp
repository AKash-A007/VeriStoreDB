#include <CLI/CLI.hpp>
#include <iostream>

int main(int argc ,char **argv) {
    CLI::App VeriStoreDB{"vsdb"} ;
    auto* greet = VeriStoreDB.add_subcommand("greet", "Greet someone");
    std::string name;
    greet->add_option("name",name,"Nme to greet")->required();
    CLI11_PARSE(VeriStoreDB, argc,argv)
    if (VeriStoreDB.got_subcommand(greet)){
        std::cout << "Hello" <<name<< "!\n";
    }
    
    return 0;
}