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

coro::generator<std::pair<operation_class, matrix>> process_arguments(coro::generator<std::string_view> gen_args)
{
    auto iterator = gen_args.begin();
    co_yield std::pair(operation_class::init, matrix(*++iterator));

    for (++iterator; iterator != gen_args.end(); ++iterator)
    {
        switch (hash::fnv1a_32(*iterator))
        {
        case hash::fnv1a_32("--add"):
            {
                co_yield std::pair(operation_class::add, matrix(*++iterator));
                continue;
            }

        case hash::fnv1a_32("--mult"):
            {
                co_yield std::pair(operation_class::mult, matrix(*++iterator));
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
}

int main(int argc, char *argv[])
try
{
    matrix result;

    for (auto& [op_class, matrix] : process_arguments(modernize_argv(argc, argv)))
    {
        switch (op_class)
        {
        case operation_class::init:
            {
                result = std::move(matrix);
                continue;
            }

        case operation_class::add:
            {
                result += matrix;
                continue;
            }

        case operation_class::mult:
            {
                result *= matrix;
                continue;
            }

        default:
            __builtin_unreachable();
        }
    }

    std::cout << result.display().rdbuf();
    return EXIT_SUCCESS;
}
catch (const std::bad_optional_access&)
{
    std::cerr << "Too few arguments" << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
