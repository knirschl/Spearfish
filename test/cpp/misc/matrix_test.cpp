//
// Created by fili on 24.07.24.
//

#include <boost/ut.hpp>
#include <vector>
#include "../../../src/cpp/misc/matrix.hpp"
#include "matrix_test.hpp"

using namespace boost::ut;

void matrix_test() {
    "empty_constr"_test = [] {
        spearfish_matrix_ltri_t<int> m{};
        expect(m.get().empty()) << m.get().size();
        expect(m.str().empty()) << m.str();
    };

    "rows_constr"_test = [] (const auto& row) {
        spearfish_matrix_ltri_t<int> m{row};
        std::stringstream s{};
        for (size_t i{}; i < row; ++i) {
            s << "0";
            for (size_t j{}; j < i; ++j) {
                s << " 0";
            }
            s << "\n";
        }
        expect(m.str() == s.str()) << m.str();
    } | std::vector{0, 1, 2, 10};

    "rows_diag_constr"_test = [] (const std::pair<int, int>& p) {
        size_t row = p.first;
        auto diag = p.second;
        spearfish_matrix_ltri_t<int> m{row, diag};
        std::stringstream s{};
        for (size_t i{}; i < row; ++i) {
            for (size_t j{}; j < i + 1; ++j) {
                if (i == j) {
                    s << diag << "\n";
                } else {
                    s << "0 ";
                }
            }
        }
        expect(m.str() == s.str()) << m.str();
    } | std::vector<std::pair<int, int>>{{0, 0}, {1, 0}, {2, 0},
        {0, 1}, {0, 1}, {1, 1}, {2, 12}};

    "push_row"_test = [] {
        spearfish_matrix_ltri_t<int> m{1};
        expect(m.str() == "0\n") << m.str();
        m.push_row(1);
        m.push_row({2, 3});
        expect(m.str() == "0\n1 0\n2 3 0\n") << m.str();
    };

    "set_row"_test = [] {
        spearfish_matrix_ltri_t<int> m{4};
        expect(m.str() == "0\n0 0\n0 0 0\n0 0 0 0\n") << m.str();
        m.set_row({1}, 1);
        m.set_row({2, 3}, 2);
        m.set_row({4, 5, 6}, 3);
        expect(m.str() == "0\n1 0\n2 3 0\n4 5 6 0\n") << m.str();
    };

}