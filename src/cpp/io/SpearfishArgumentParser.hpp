#ifndef SPEARFISH_SPEARFISH_ARGUMENT_PARSER_HPP
#define SPEARFISH_SPEARFISH_ARGUMENT_PARSER_HPP

#include "/home/balin/Documents/Programming/libs/argparse/include/argparse/argparse.hpp"
//#include "/home/fili/Documents/Programming/libs/argparse/include/argparse/argparse.hpp"
//#include "/hits/basement/cme/knirsch/github/argparse/include/argparse/argparse.hpp"

class SpearfishArgumentParser {
private:
    argparse::ArgumentParser parser;

    void init();

public:
    SpearfishArgumentParser(const std::string& exec_name, const std::string& version);

    bool parse(const int argc, char** argv);

    std::string get_species_matrix();
    
    std::string get_alignment_matrix();

    bool has_starting_tree();
    
    std::string get_starting_tree();
    
    int get_algo();
    
    int get_c();
    
    std::string get_output_prefix();

    bool has_mapping_config();
    
    std::tuple<std::string, bool, std::string> get_mapping_config();
};

#endif // SPEARFISH_SPEARFISH_ARGUMENT_PARSER_HPP
