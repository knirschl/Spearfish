//
// Created by knirschl on 18.04.23.
//

#include <sstream>
#include <iomanip>
#include "io/parse_cli.h"
#include "io/parse_file.h"
#include "io/write_tree.h"
#include "nj/tree.h"
#include "nj/matrix.h"
#include "nj/NJSimple.h"
#include "misc/meta.h"

/**
 * Resets tree and mappings that need to be reset.
 *
 * ==========
 * INFO: Only needed for figuring out what settings are best
 * ==========
 *
 * @param map_config
 * @return
 */
auto reset(const std::vector<std::string> &species_tree_ids,
           const std::vector<std::string> &alignment_ids,
           const std::tuple<std::string, bool, std::string> &map_config) {
    // fill map
    if (get<0>(map_config).empty()) {
        // no mapping provided
        for (int i{}; i < alignment_ids.size(); i++) {
            leafname2groupname.emplace(alignment_ids[i], species_tree_ids[i]);
        }
    } else {
        leafname2groupname = parse_mapping_from_cfg(map_config);
    }
    // create tree
    std::shared_ptr<Tree> tree = std::make_shared<Tree>();
    tree->make_leafs(alignment_ids);
    return tree;
}

/**
 * Run nj, reroot, nj.
 *
 * ==========
 * INFO: Only needed for figuring out what settings are best
 * ==========
 *
 * @param scale
 * @param tree
 * @param active
 * @param species_tree_mat
 * @param alignment_mat
 * @param cli_parser
 */
void run(double scale, const std::shared_ptr<Tree> &tree, const std::vector<int> &active,
         const std::vector<std::vector<double>> &species_tree_mat,
         const std::vector<std::vector<double>> &alignment_mat,
         argparse::ArgumentParser &cli_parser) {
    matrix_t<double> scaleMatrix{};
    matrix_t<double> sumMatrix{};
    matscale(species_tree_mat, scale, scaleMatrix);
    matadd(alignment_mat, scaleMatrix, sumMatrix);
    //std::cout << "Scaled Species-Tree-Matrix + Alignment-Matrix =\n" << matstr(distMatrix) << "\n\n";
    int root = neighborJoining<>(sumMatrix, tree, active);
    std::cout << "Neighbor-joined tree: " << tree->to_newick() << std::endl;

    // TODO tag and reroot
    // TODO change selected alignment_mat entries
    // TODO nj again

    // double to string without trailing zeros
    std::ostringstream oss;
    oss << std::setprecision(8) << std::noshowpoint << scale;
    write_newick(*tree, getP(cli_parser) + oss.str() + "S+G.geneTree.newick");
}

int main(int argc, char *argv[]) {
    auto cli_parser = build_parser("thesis", "0.1");
    parse(cli_parser, argc, argv);
    //std::cout << getS(cli_parser) << "\n" << getA(cli_parser) << "\n" << getP(cli_parser) << "\n";

    // read species tree
    auto species_tree_pair = parse_phylip_mat_from_file<dist_t>(getS(cli_parser));
    auto species_tree_mat = species_tree_pair.first;
    auto species_tree_ids = species_tree_pair.second;

    // read alignment
    auto alignment_pair = parse_phylip_mat_from_file<dist_t>(getA(cli_parser));
    auto alignment_mat = alignment_pair.first;
    auto alignment_ids = alignment_pair.second;

    auto map_config{getMappingConfig(cli_parser)};

    // calculate
    {
        double div{100.0};
        int step{5};
        for (int i{}; i < 1 * div; i += step) {
            std::shared_ptr<Tree> tree = reset(species_tree_ids, alignment_ids, map_config);
            std::vector<int> active{leaf_indices};

            if (i == 1) {
                step = 25;
            }
            run(i / div, tree, active, species_tree_mat, alignment_mat, cli_parser);
        }
        step = 25;
        for (int i{int(1 * div)}; i <= 2 * div; i += step) {
            std::shared_ptr<Tree> tree = reset(species_tree_ids, alignment_ids, map_config);
            std::vector<int> active{leaf_indices};

            run(i / div, tree, active, species_tree_mat, alignment_mat, cli_parser);
        }
        for (int i{int(2.5 * div)}; i <= 10 * div; i *= 2) {
            std::shared_ptr<Tree> tree = reset(species_tree_ids, alignment_ids, map_config);
            std::vector<int> active{leaf_indices};

            run(i / div, tree, active, species_tree_mat, alignment_mat, cli_parser);
        }
    }

    return 0;
}


