//
// Created by knirschl on 25.05.23.
//

#ifndef BA_WRITE_FILE_H
#define BA_WRITE_FILE_H

#include <fstream>
#include <iostream>
#include "../nj/tree.h"
#include "../misc/common_types.h"

bool write_newick(Tree const &tree, std::ofstream &writer) {
    if (writer.is_open() && writer.good()) {
        writer << tree.to_newick() << '\n';
    }
    return !writer.fail();
}

bool write_newick(Tree const &tree, std::string const &out_file) {
    std::ofstream streamed_file{out_file};
    bool is_written = write_newick(tree, streamed_file);
    streamed_file.close();
    return is_written;
}

bool write_phylip(const dist_matrix_t &matrix, const vector_t<std::string> &names, std::ofstream &writer, bool enforce_phylip) {
    if (names.size() != matrix.size()) {
        std::cout << "Matrix and names don't match in size.\n";
    }
    size_t max_name_len{10}; // standard phylip
    for (auto& name : names) {
        auto name_len{name.length()};
        max_name_len = name_len > max_name_len ? name_len : max_name_len;
    }
    if (max_name_len > 10 && enforce_phylip) {
        std::cout << "At least one locus name is longer than 10 characters. Cannot output strict phylip matrix.\n";
        return false;
    }
    const size_t ms{matrix.size()};
    if (writer.is_open() && writer.good()) {
        writer << ms;
        // FastME accepts up to 64 characters
        //if (!enforce_phylip && max_name_len > 10) {
        //    writer << ' ' << std::to_string(max_name_len);
        //}
        writer << '\n';
        for (int row{}; row < ms; row++) {
            // one-time padding
            writer << std::setw(max_name_len + 1) << names[row];
            for (auto &col: matrix[row]) {
                writer << col << ' ';
            }
            writer << '\n';
        }
    }
    return !writer.fail();
}

bool write_phylip(const dist_matrix_t &matrix, const vector_t<std::string> &names, const std::string &out_file, bool enforce_phylip = false) {
    std::ofstream streamed_file{out_file};
    // decimal places           << keep word on left, fill chars to right
    streamed_file << std::fixed << std::setprecision(17) << std::left;
    bool is_written = write_phylip(matrix, names, streamed_file, enforce_phylip);
    streamed_file.close();
    return is_written;
}

#endif //BA_WRITE_FILE_H
