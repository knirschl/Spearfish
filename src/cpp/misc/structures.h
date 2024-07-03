//
// Created by knirschl on 03.07.24
//

#ifndef SPEARFISH_STRUCTURES_H
#define SPEARFISH_STRUCTURES_H

#include <vector>
#include <string>

template<typename T>
class TriMatrix {
private:
    std::vector<T> matrix;
    T diag{};
    std::size_t rows;

    inline std::size_t ltri_index(const std::size_t &r, const std::size_t &c) {
        return ((r * (r - 1)) >> 1) + c;
    }

public:
    LTriMatrix() : matrix{}, rows{} {}

    LTriMatrix(const std::size_t &rows) : rows{rows},
                        matrix((rows * (rows - 1)) >> 1) {}

    T& operator[](std::size_t r, std::size_t c) {
        if (this->rows < 0) {
            // TODO  failure
            exit(-1);
        }
        if (r < c) {
            std::swap(r, c);
        } 
        const std::size_t idx{ltri_index(r, c)};
        if (idx > ltri_index(rows, 0)) {
            // TODO  failure
            exit(-1);
        }
        if (r == c) {
            return this->diag;
        }
        return this->matrix[idx];
    }
    
    const T& operator[](std::size_t &r, std::size_t &c) {
        if (this->rows < 0) {
            // TODO  failure
            exit(-1);
        }
        if (r < c) {
            std::swap(r, c);
        } 
        const std::size_t idx{ltri_index(r, c)};
        if (idx > ltri_index(rows, 0)) {
            // TODO  failure
            exit(-1);
        }
        if (r == c) {
            return this->diag;
        }
        return this->matrix[idx];
    }

    template<typename... U>
    void push_row(const U&&... v) {
        // TODO failure if U != T
        constexpr std::size_t n = sizeof...(U);
        if (n != this->rows) {
            // TODO  failure
            return;
        }
        (this->matrix.push_back(v), ...);
        this->rows++;
    }

    const std::vector<T>& get() {
        return this->matrix;
    }
    
    std::string str(const bool& diag = false) {
        if (this->rows < 0) {
            return "";
        }
        std::string s{""};
        for (std::size_t r{!diag}; r < this->rows; r++) {
            for (std::size_t c{}; c < r + diag; c++) {
                if (diag & r == c) {
                    s += std::to_string(this->diag);
                    continue;
                }
                s += std::to_string(this->matrix[ltri_index(r, c)]);
                s += " ";
            }
            s += "\n";
        }
        return s;
    }
};

#endif // SPEARFISH_STRUCTURES_H
