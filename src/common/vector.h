#pragma once
#include <memory>
#include <type_traits>

namespace simpl
{
    template <typename T, typename Alloc = std::allocator<T>>
    requires std::copyable<T> || std::movable<T>
    class kektor
    {
        using traits = typename std::allocator_traits<Alloc>::template rebind_traits<T>;
        using allocator = typename traits::allocator_type;

    public:
        using difference_type = typename traits::difference_type;
        using size_type = typename traits::size_type;
        using value_type = typename traits::value_type;
        using pointer = typename traits::pointer;
        using const_pointer = typename traits::const_pointer;
        using reference = std::add_lvalue_reference_t<value_type>;
        using const_reference = std::add_const_t<reference>;

    private:
        allocator alloc_;
        pointer data_ = nullptr;
        size_type size_ = 0;
        size_type capacity_ = 0;

    public:
        kektor() = default;

        template <typename... Args>
        explicit kektor(size_type n, Args&&... args)
        {
            resize(n, std::forward<Args>(args)...);
        }

        kektor(const kektor& rhs)
        {
            *this = rhs;
        }

        kektor& operator=(const kektor& rhs)
        {
            if (this != std::addressof(rhs))
            {
                auto new_data = traits::allocate(alloc_, rhs.capacity_);
                std::uninitialized_copy_n(rhs.data_, rhs.size_, new_data);
                deallocate();

                data_ = new_data;
                size_ = rhs.size_;
                capacity_ = rhs.capacity_;
            }

            return *this;
        }

        kektor(kektor&& rhs) noexcept
        {
            *this = std::move(rhs);
        }

        kektor& operator=(kektor&& rhs) noexcept
        {
            if (this != std::addressof(rhs))
            {
                std::swap(data_, rhs.data_);
                std::swap(size_, rhs.size_);
                std::swap(capacity_, rhs.capacity_);
            }

            return *this;
        }

        void reserve(size_type n)
        {
            if (n <= capacity_)
            {
                return;
            }

            auto new_data = traits::allocate(alloc_, n);
            std::uninitialized_copy_n(data_, size_, new_data);
            deallocate();

            data_ = new_data;
            capacity_ = n;
        }

        template <typename... Args>
        void resize(size_type n, Args&&... args)
        {
            if (n <= size_)
            {
                std::destroy_n(data_ + n, size_ - n);
                size_ = n;
                return;
            }

            do
            {
                if (n <= capacity_)
                {
                    std::uninitialized_fill_n(data_ + size_, n - size_, T(std::forward<Args>(args)...));
                    size_ = n;
                    return;
                }

                reserve(n);
            }
            while (true);
        }

        template <typename U = T>
        void push_back(U&& value)
        {
            if (size_ == capacity_)
            {
                reserve(capacity_ == 0 ? 1 : capacity_ << 1);
            }

            std::construct_at(std::addressof(data_[size_]), std::forward<U>(value));
            ++size_;
        }

        void pop_back()
        {
            if (size_ == 0)
            {
                throw std::out_of_range("pop_back(): size == 0");
            }

            std::destroy_at(std::addressof(data_[size_ - 1]));
            --size_;
        }

        reference operator[](size_type index)
        {
            if (index >= size_)
            {
                throw std::out_of_range("operator[]: index out of range");
            }

            return data_[index];
        }

        const_reference operator[](size_type index) const
        {
            return const_cast<kektor&>(*this)[index];
        }

        [[nodiscard]] size_type size() const noexcept
        {
            return size_;
        }

        const_pointer begin() const noexcept
        {
            return data_;
        }

        const_pointer end() const noexcept
        {
            return data_ + size_;
        }

        pointer begin() noexcept
        {
            return data_;
        }

        pointer end() noexcept
        {
            return data_ + size_;
        }

        ~kektor()
        {
            deallocate();
        }

    private:
        void deallocate()
        {
            if (data_)
            {
                std::destroy_n(data_, size_);
                traits::deallocate(alloc_, data_, capacity_);
            }

            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    };
}
