//
// Created by knirschl on 03.07.24
//

#ifndef SPEARFISH_MISC_MATRIX_HPP
#define SPEARFISH_MISC_MATRIX_HPP

#include <vector>
#include <string>
#include <stdexcept>

template<typename T>
class spearfish_matrix_ltri_t {
public:
    spearfish_matrix_ltri_t() = default;

    explicit spearfish_matrix_ltri_t(const size_t rows, const T diag = 0) :
        matrix(len(rows)),
        rows{rows} {
            if (rows) {
                matrix[0] = diag;
            }
    }

    decltype(auto) operator[](this auto& self, std::size_t r, size_t c) {
        if (self.rows < 0) {
            throw std::runtime_error{
                "Invalid index while accessing spearfish_matrix_ltri_t"};
        }
        if (r < c) {
            std::swap(r, c);
        }
        if (c < 0 || r >= self.rows) {
            throw std::runtime_error{
                "Invalid index while accessing spearfish_matrix_ltri_t"};
        }
        return self.matrix[idx(r, c)];
    }

    void set_row(this auto& self, const std::vector<T>& vs, const size_t row) {
        if (vs.size() != row) {
            return; // TODO failure
        }
        if (row > self.rows) {
            self.push_row(vs); // try
        }
        for (int c{}; c < row; ++c) {
            self[row, c] = vs[c];
        }
    }

    void push_row(const std::vector<T>& vs) {
        if (vs.size() != this->rows) {
            return; // TODO failure
        }
        // O(|matrix| + |vs|)
        /*
        this->matrix.insert(this->matrix.end(),
            std::make_move_iterator(vs.begin()),
            std::make_move_iterator(vs.end())
        );
        */
        //this->matrix.reserve(vs.size());
        for (auto&& v : vs) {
            this->matrix.push_back(v);
        }
        ++this->rows;
    }

    template<typename ...U>
    void push_row(const U&...v) {
        // TODO failure if U != T
        constexpr size_t n = sizeof...(U);
        if (n != this->rows) {
            // TODO  failure
            return;
        }
        (this->matrix.push_back(v), ...);
        ++this->rows;
    }

    const std::vector<T>& get() {
        return this->matrix;
    }

    std::string str() {
        if (this->rows < 0) {
            return "";
        }
        std::stringstream s{};
        for (size_t r{}; r < this->rows; ++r) {
            for (size_t c{}; c < r + 1; ++c) {
                s << std::to_string(this->matrix[idx(r, c)]);
                s << (r == c ? "\n" : " ");
            }
        }
        return s.str();
    }

private:
    static size_t len(const size_t r) {
        return (r * r + r) >> 1;
    }
    
    static size_t idx(const size_t r, const size_t c) {
        return r == c ? 0 : ((r * (r - 1)) >> 1) + c + 1;
    }

    std::vector<T> matrix;
    size_t rows{};
};

#endif // SPEARFISH_MISC_MATRIX_HPP
