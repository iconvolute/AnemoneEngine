#pragma once
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <span>
#include <utility>
#include <iterator>
#include <memory>

namespace Anemone::Interop
{
    // Dynamic array container. Provides a simple interface for dynamic arrays.
    // It is not a replacement for std::vector, but a simple wrapper around raw pointers.
    // It's better than unique_ptr<T[]> because it knows the size of the array.
    template <typename T>
    class dynamic_array
    {
    public:
        using value_type = T;
        using reference = T&;
        using const_reference = T const&;
        using pointer = T*;
        using const_pointer = T const*;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        dynamic_array() = default;

        dynamic_array(dynamic_array const&) = delete;

        dynamic_array(dynamic_array&& other) noexcept
            : _data{std::exchange(other._data, nullptr)}
            , _size{std::exchange(other._size, 0)}
        {
        }

        dynamic_array(size_type count)
            : dynamic_array{count, value_type{}}
        {
        }

        dynamic_array(size_type count, value_type const& value)
        {
            if (T* elements = this->allocate_uninitialized(count))
            {
                this->_data = elements;
                this->_size = count;

                std::uninitialized_fill_n(this->_data, this->_size, value);
            }
        }

        ~dynamic_array()
        {
            this->clear();
        }

        dynamic_array& operator=(dynamic_array const&) = delete;

        dynamic_array& operator=(dynamic_array&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->clear();
                this->_data = std::exchange(other._data, nullptr);
                this->_size = std::exchange(other._size, 0);
            }
            return *this;
        }

        reference operator[](size_t index)
        {
            AE_ASSERT(index < this->_size);
            return this->_data[index];
        }

        const_reference operator[](size_t index) const
        {
            AE_ASSERT(index < this->_size);
            return this->_data[index];
        }

        bool empty() const
        {
            return this->_size == 0;
        }

        size_t size() const
        {
            return this->_size;
        }

        pointer data()
        {
            return this->_data;
        }

        const_pointer data() const
        {
            return this->_data;
        }

        reference front()
        {
            AE_ASSERT(this->_size > 0);
            return this->_data[0];
        }

        const_reference front() const
        {
            AE_ASSERT(this->_size > 0);
            return this->_data[0];
        }

        reference back()
        {
            AE_ASSERT(this->_size > 0);
            return this->_data[this->_size - 1];
        }

        const_reference back() const
        {
            AE_ASSERT(this->_size > 0);
            return this->_data[this->_size - 1];
        }

        iterator begin()
        {
            return this->_data;
        }

        iterator end()
        {
            return this->_data + this->_size;
        }

        const_iterator begin() const
        {
            return this->_data;
        }

        const_iterator end() const
        {
            return this->_data + this->_size;
        }

        const_iterator cbegin() const
        {
            return this->_data;
        }

        const_iterator cend() const
        {
            return this->_data + this->_size;
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator{this->end()};
        }

        reverse_iterator rend()
        {
            return reverse_iterator{this->begin()};
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator{this->end()};
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator{this->begin()};
        }

        const_reverse_iterator crbegin() const
        {
            return const_reverse_iterator{this->end()};
        }

        const_reverse_iterator crend() const
        {
            return const_reverse_iterator{this->begin()};
        }

        void resize(size_type size, value_type const& value)
        {
            if (this->_size != size)
            {
                if (T* elements = this->allocate_uninitialized(size))
                {
                    if (this->_data)
                    {
                        // Move the old data to the new location.
                        std::uninitialized_move_n(this->_data, std::min(this->_size, size), elements);

                        // Fill the rest of the new data with the value.
                        if (size > this->_size)
                        {
                            std::uninitialized_fill_n(elements + this->_size, size - this->_size, value);
                        }

                        // Destroy the old data.
                        std::destroy(this->_data, this->_data + this->_size);
                        ::operator delete(this->_data, std::align_val_t{alignof(T)});
                    }
                    else
                    {
                        std::uninitialized_fill_n(elements, size, value);
                    }

                    this->_data = elements;
                    this->_size = size;
                }
            }
        }

        void clear()
        {
            if (this->_data)
            {
                std::destroy(this->_data, this->_data + this->_size);
                ::operator delete(this->_data, std::align_val_t{alignof(T)});
                this->_data = nullptr;
                this->_size = 0;
            }
        }

        void swap(dynamic_array& other) noexcept
        {
            std::swap(this->_data, other._data);
            std::swap(this->_size, other._size);
        }

        std::span<T> as_span()
        {
            return std::span{this->_data, this->_size};
        }

        std::span<T const> as_span() const
        {
            return std::span{this->_data, this->_size};
        }

        std::span<std::byte> as_bytes()
        {
            return std::span{reinterpret_cast<std::byte*>(this->_data), this->_size * sizeof(T)};
        }

        std::span<std::byte const> as_bytes() const
        {
            return std::span{reinterpret_cast<std::byte const*>(this->_data), this->_size * sizeof(T)};
        }

    private:
        T* allocate_uninitialized(size_type count)
        {
            AE_ASSERT(this->empty());

            if (count != 0)
            {
                return static_cast<T*>(::operator new(sizeof(T) * count, std::align_val_t{alignof(T)}));
            }

            return nullptr;
        }

    private:
        T* _data{};
        size_t _size{};
    };
}
