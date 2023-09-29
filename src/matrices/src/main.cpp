#include <iostream>
#include <span>
#include <sstream>
#include <string_view>
#include <vector>

#include "generator.h"
#include "hash.h"
#include "matrices.hpp"

coro::generator<std::string_view> modernize_argv(int argc, char *argv[])
{
    for (const auto arg : std::span(argv, argc))
    {
        co_yield arg;
    }
}

enum class operation_class
{
    init, add, mult
};

auto process_arguments(coro::generator<std::string_view> gen_args)
{
    auto iterator = gen_args.begin();

    std::vector<std::pair<operation_class, std::string_view>> result{
        { operation_class::init, *++iterator }
    };

    for (++iterator; iterator != gen_args.end(); ++iterator)
    {
        switch (hash::fnv1a_32(*iterator))
        {
        case hash::fnv1a_32("--add"):
            {
                result.emplace_back(operation_class::add, *++iterator);
                continue;
            }

        case hash::fnv1a_32("--mult"):
            {
                result.emplace_back(operation_class::mult, *++iterator);
                continue;
            }

        default:
            {
                std::stringstream ss;
                ss << "Unknown argument: " << *iterator;

                throw std::runtime_error(ss.str());
            }
        }
    }

    return result;
}

int main(int argc, char *argv[])
{
    matrix result;

    for (const auto& [op_class, mat_file] : process_arguments(modernize_argv(argc, argv)))
    {
        switch (op_class)
        {
        case operation_class::init:
            {
                result = matrix(mat_file);
                continue;
            }

        case operation_class::add:
            {
                result += matrix(mat_file);
                continue;
            }

        case operation_class::mult:
            {
                result *= matrix(mat_file);
                continue;
            }

        default:
            __builtin_unreachable();
        }
    }

    std::cout << result.display().rdbuf();
}
