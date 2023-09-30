#include <fstream>
#include <sstream>

#include "matrices.hpp"

matrix::matrix(const std::filesystem::path& file)
{
    std::ifstream in;
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    in.open(file);

    in >> dim_row_;
    in >> dim_col_;
    mat_data_.resize(dim_row_ * dim_col_, 0);

    for (double& val : mat_data_)
    {
        in >> val;
    }
}

matrix& matrix::operator+=(const matrix& rhs)
{
    if (dim_row_ != rhs.dim_row_ || dim_col_ != rhs.dim_col_)
    {
        std::stringstream ss;
        ss << "Mismatched dimensions: ";
        ss << dim_row_ << "x" << dim_col_;
        ss << " + ";
        ss << rhs.dim_row_ << "x" << rhs.dim_col_;

        throw std::runtime_error(ss.str());
    }

    for (size_t i = 0; i < dim_row_ * dim_col_; ++i)
    {
        mat_data_[i] += rhs.mat_data_[i];
    }

    return *this;
}

matrix& matrix::operator*=(const matrix& rhs)
{
    if (dim_col_ != rhs.dim_row_)
    {
        std::stringstream ss;
        ss << "Mismatched dimensions: ";
        ss << dim_row_ << "x" << dim_col_;
        ss << " * ";
        ss << rhs.dim_row_ << "x" << rhs.dim_col_;

        throw std::runtime_error(ss.str());
    }

    matrix result(dim_row_, rhs.dim_col_);

    for (size_t i = 0; i < dim_row_; ++i)
    {
        for (size_t j = 0; j < dim_col_; ++j)
        {
            for (size_t k = 0; k < rhs.dim_col_; ++k)
            {
                result(i, k) += (*this)(i, j) * rhs(j, k);
            }
        }
    }

    *this = std::move(result);
    return *this;
}

std::stringstream matrix::display() const
{
    std::stringstream ss;
    ss << dim_row_ << " " << dim_col_;
    ss << std::endl;

    for (size_t i = 1; i < dim_row_ * dim_col_ + 1; ++i)
    {
        ss << mat_data_[i - 1];

        if (i % dim_col_ != 0)
        {
            ss << " ";
        }
        else
        {
            ss << std::endl;
        }
    }

    return ss;
}
