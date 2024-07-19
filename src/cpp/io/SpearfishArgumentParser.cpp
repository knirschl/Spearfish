#include "SpearfishArgumentParser.hpp"

void SpearfishArgumentParser::init() {
    // TODO description and epilog
    this->parser.add_description("");
    this->parser.add_epilog("");
    
    this->parser.add_argument("-s", "--species-mat")
            .required()
            .help("specify the species tree distance matrix in PHYLIP format");
    
    this->parser.add_argument("-a", "--align-mat")
            .required()
            .help("specify the gene alignment distance matrix in PHYLIP format");
    
    this->parser.add_argument("-t", "--starting-tree")
            .help("specify the gene tree to compute dup/loss on (NEWICK format)");
    
    // r := tag and reroot algorithm
    this->parser.add_argument("-r", "--tag-algo")
            .default_value(0)
            .scan<'i', int>()
            .help("0: APro, 1: MAD, 2: none (everything gets corrected (S+G))");
    
    // c := compute
    this->parser.add_argument("-c", "--compute-tree")
            .default_value(2)
            .scan<'i', int>()
            .help("0: only tree, 1: both, 2: only matrix");
    
    this->parser.add_argument("-p", "--prefix")
            .default_value(std::string{"/"})
            .help("specify the output prefix");
    
    this->parser.add_argument("-m", "--map")
            .default_value(std::string{""})
            .help("specify the mapping between species names and locus names");
    
    // direction s->l or l->s
    this->parser.add_argument("-l")
            .default_value(false)
            .implicit_value(true)
            .help("if set, mapping file maps locus names to species names");
    
    // delimiter between species and locus names
    this->parser.add_argument("-d", "--delimiter")
            .default_value(std::string{":"})
            .help("mapping separator");
}

SpearfishArgumentParser::SpearfishArgumentParser(const std::string& exec_name, const std::string& version)
    : parser{exec_name, version} {
        init();
}

bool SpearfishArgumentParser::parse(const int argc, char** argv) {
    try {
        parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << this->parser;
        return 1;
    }
    return 0;
}

std::string SpearfishArgumentParser::get_species_matrix() {
    return this->parser.get("-s");
}

std::string SpearfishArgumentParser::get_alignment_matrix() {
    return this->parser.get("-a");
}

bool SpearfishArgumentParser::has_starting_tree() {
    return this->parser.is_used("-t");
}

std::string SpearfishArgumentParser::get_starting_tree() {
    return this->parser.get("-t");
}

int SpearfishArgumentParser::get_algo() {
    return this->parser.get<int>("-r");
}

int SpearfishArgumentParser::get_c() {
    return this->parser.get<int>("-c");
}

std::string SpearfishArgumentParser::get_output_prefix() {
    return this->parser.get("-p");
}

bool SpearfishArgumentParser::has_mapping_config() {
    return this->parser.is_used("-m");
}

std::tuple<std::string, bool, std::string> SpearfishArgumentParser::get_mapping_config() {
    return {this->parser.get("-m"), this->parser.get<bool>("-l"), this->parser.get("-d")};
}