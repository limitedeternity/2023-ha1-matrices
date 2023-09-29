#pragma once
#include <concepts>
#include <string_view>

namespace hash
{
    template <std::integral I, I Prime, I Offset>
    constexpr I fnv1a_impl(const char* s, const size_t size) noexcept
    {
        I hash = Offset;

        for (size_t i = 0; i < size; ++i)
        {
            hash = hash ^ static_cast<I>(s[i]);
            hash = hash * Prime;
        }

        return hash;
    }

    constexpr auto fnv1a_32(const char* s, const size_t size)
    {
        return fnv1a_impl<uint32_t, 16777619, 2166136261>(s, size);
    }

    constexpr auto fnv1a_32(const std::string_view s)
    {
        return fnv1a_32(s.data(), s.size());
    }
}
