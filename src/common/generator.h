#pragma once
// ReSharper disable CppMemberFunctionMayBeStatic

#include <concepts>
#include <coroutine>
#include <iterator>
#include <optional>

namespace coro
{
    template <typename T>
    class generator
    {
    public:
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            std::optional<T> current_value;

            generator get_return_object()
            {
                return generator(handle_type::from_promise(*this));
            }

            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            void return_void() { current_value = {}; }
            void unhandled_exception() { throw; }

            template <std::convertible_to<T> From>
            std::suspend_always yield_value(From&& from) noexcept
            {
                if constexpr (std::same_as<T, From>)
                {
                    current_value = std::forward<From>(from);
                }
                else
                {
                    current_value = T(std::forward<From>(from));
                }

                return {};
            }

            void await_transform() = delete;
        };

        explicit generator(const handle_type coro) :
            coroutine_{ coro }
        {}

        ~generator()
        {
            if (coroutine_)
            {
                coroutine_.destroy();
            }
        }

        generator(const generator&) = delete;
        generator& operator=(const generator&) = delete;

        generator(generator&& other) noexcept :
            coroutine_{ other.coroutine_ }
        {
            other.coroutine_ = {};
        }

        generator& operator=(generator&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                if (coroutine_)
                {
                    coroutine_.destroy();
                }

                coroutine_ = other.coroutine_;
                other.coroutine_ = {};
            }

            return *this;
        }

        class iter
        {
            handle_type coroutine_;

        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = T&;

            explicit iter(const handle_type coro) :
                coroutine_{ coro }
            {}

            iter& operator++()
            {
                coroutine_.resume();
                return *this;
            }

            void operator++(int) {
                (void)operator++();
            }

            reference operator*() const
            {
                if (auto& current_value = coroutine_.promise().current_value)
                {
                    return *current_value;
                }

                throw std::bad_optional_access();
            }

            bool operator==(std::default_sentinel_t) const noexcept
            {
                return !coroutine_ || coroutine_.done();
            }
        };

        iter begin()
        {
            if (coroutine_)
            {
                coroutine_.resume();
            }

            return iter{ coroutine_ };
        }

        [[nodiscard]] std::default_sentinel_t end() const { return {}; }

    private:
        handle_type coroutine_;
    };
}
