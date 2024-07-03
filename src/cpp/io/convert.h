//
// Created by knirschl on 02.07.24
//

#ifndef SPEARFISH_CONVERT_H
#define SPEARFISH_CONVERT_H

#include <fstream>
#include <vector>
#include <utility>
#include <type_traits>
#include <typeinfo>
#include <string>
#include "../misc/common_types.h"

bool is_valid_number_char(char const& c) {
    return (c >= '0' && c <= '9') || (c == '.') || (c == '-');
}

bool is_valid_id_char(char const& c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')
        //|| is_valid_number_char(c)
        ;
}

template<typename T>
T stoT(std::string const& s) {
    if (std::is_same<T, double>) {
        return std::stod(s);
    } else if (std::is_same<T, long double>) {
        return std::stold(s);
    } else if (std::is_same<T, float>) {
        return std::stof(s);
    } else if (std::is_same<T, int>) {
        return std::stoi(s);
    } else if (std::is_same<T, long>) {
        return std::stol(s);
    } else if (std::is_same<T, long long>) {
        return std::stoll(s);
    } else if (std::is_same<T, unsigned long>) {
        return std::stoul(s);
    } else if (std::is_same<T, unsigned long long>) {
        return std::stoull(s);
    } else {
        try {
            return T(s);
        } catch (...) {
            // TODO
            throw bad_cast();
        }
    }
}

template<typename T>
void add(matrix_t<T>& dist_matrix, vector_t<T> row_placeholder,
        T const& val, std::size_t const& idx, std::size_t const& cover_range = -1) {
    // TODO + row vector
    // add row/column if cover_range = -1
    // add idx to every entry [idx,i] and [i,idx] (not [idx,idx])
    // but not [idx,x] if x >= cover_range && x <= idx
    if (cover_range == -1) {
        // add bl of one node to all respective distances
        dist_matrix.push_back(vector_t<T>{});
    }
}

template<typename T>
std::pair<matrix_t<T>, vector_t<std::string>> convert_nwk_dm(std::ifstream &reader) {
    std::size_t idx{}; // same as rightmost node of the current subtree
    std::string tmp{""};
    std::stack<std::size_t> subtree_start{};
    matrix_t<T> dist_matrix{};
    vector_t<T> row_placeholder{};
    vector_t<std::string> ids{};

    // TODO include check for correct newick
    while (reader.good() && char c{reader.get()} != ';') {
        if (c == '(') {
            // start of subtree: store leftmost node index
            subtree_start.push(idx);
        } else if (is_valid_number_char(c) || is_valid_id_char(c)) {
            tmp += c;
        } else if (c == ':' && tmp != "") {
            // id fully read
            ids.push_back(tmp)
            tmp.clear();
        } else if (c == ',') {
            // branch length fully read: add it to matrix
            // new node (idx) next
            add<T>(dist_matrix, row_placeholder, stoT<T>(tmp), idx++);
            tmp.clear();
        } else if (c == ')') {
            // end of subtree: add branch length to matrix
            // but exclude distances where both nodes are inside this subtree 
            add<T>(dist_matrix, row_placeholder, stoT<T>(tmp), idx, subtree_start.pop());
            tmp.clear();
        }
    }

    return {dist_matrix, ids};
}

#endif // SPEARFISH_CONVERT_H
