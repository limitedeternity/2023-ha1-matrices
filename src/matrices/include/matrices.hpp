#pragma once
// ReSharper disable CppMemberFunctionMayBeStatic

#include <filesystem>
#include <sstream>

#include "vector.h"

class matrix
{
    simpl::kektor<double> mat_data_;
    size_t dim_row_ = 0;
    size_t dim_col_ = 0;

public:
    matrix() = default;
    explicit matrix(const std::filesystem::path& file);

    matrix(const size_t row, const size_t col) :
        mat_data_(row * col),
        dim_row_{row},
        dim_col_{col}
    {}

    matrix(const matrix&) = default;
    matrix& operator=(const matrix&) = default;

    matrix(matrix&&) noexcept = default;
    matrix& operator=(matrix&&) noexcept = default;

    ~matrix() = default;

    double& operator()(const size_t row, const size_t col)
    {
        return mat_data_[idx(row, col, dim_col_)];
    }

    double operator()(const size_t row, const size_t col) const
    {
        return const_cast<matrix&>(*this)(row, col);
    }

    matrix& operator+=(const matrix& rhs);
    matrix& operator*=(const matrix& rhs);

    [[nodiscard]] std::stringstream display() const;

private:
    [[nodiscard]] size_t idx(const size_t i, const size_t j, const size_t stride) const
    {
        return i * stride + j;
    }
};
