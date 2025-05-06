#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <memory>
#include <span>
#include <cassert>

namespace Anemone::Interop
{
    // A memory buffer used for low-level memory optimizations.
    // Allows to use a static buffer of a given size, and if the size is exceeded,
    // it will allocate a dynamic buffer of the required size.
    template <size_t StaticCapacityT>
    struct memory_buffer final
    {
    private:
        std::byte* m_data{};
        std::unique_ptr<std::byte[]> m_dynamic{};
        size_t m_capacity{StaticCapacityT};
        size_t m_size{};
        std::byte m_static[StaticCapacityT];

    public:
        constexpr memory_buffer()
            : m_data{m_static}
        {
        }

        constexpr explicit memory_buffer(size_t size, std::byte fill = std::byte{})
            : m_data{m_static}
        {
            this->resize_for_override(size);
            this->m_size = size;

            std::fill(this->m_data, this->m_data + size, fill);
        }

        memory_buffer(memory_buffer const&) = delete;

        memory_buffer(memory_buffer&&) = delete;

        memory_buffer& operator=(memory_buffer const&) = delete;

        memory_buffer& operator=(memory_buffer&&) = delete;

        ~memory_buffer() = default;

        constexpr void resize_for_override(size_t size)
        {
            if (size < StaticCapacityT)
            {
                // Reset back to static buffer.
                this->m_dynamic = nullptr;
                this->m_data = this->m_static;
                this->m_capacity = StaticCapacityT;
            }
            else if (size > this->m_capacity)
            {
                // Allocate dynamic buffer.
                this->m_dynamic = std::make_unique_for_overwrite<std::byte[]>(size);
                this->m_data = this->m_dynamic.get();
                this->m_capacity = size;
            }

            this->m_size = size;
        }

        constexpr void resize(size_t size)
        {
            assert(this->m_capacity >= StaticCapacityT);
            assert(this->m_capacity >= this->m_size);

            if (size != this->m_size)
            {
                if (size <= this->m_capacity)
                {
                    // Allocated buffer can hold the new size.
                    this->m_size = size;
                }
                else
                {
                    // Size exceeds capacity of the buffer. Need to allocate a new buffer, copy data and discard the previous buffer.
                    size_t const capacity = std::max(size, this->m_capacity * 2uz);

                    // Allocate new buffer.
                    std::unique_ptr<std::byte[]> newBuffer = std::make_unique_for_overwrite<std::byte[]>(capacity);

                    // Copy data from the old buffer to the new one.
                    std::copy(this->m_data, this->m_data + this->m_size, newBuffer.get());

                    std::fill(newBuffer.get() + this->m_size, newBuffer.get() + size, std::byte{});

                    this->m_data = newBuffer.get();
                    this->m_dynamic = std::move(newBuffer);
                    this->m_capacity = capacity;
                    this->m_size = size;
                }
            }
        }

        constexpr void clear()
        {
            this->m_size = 0;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return this->m_capacity;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return this->m_size;
        }

        [[nodiscard]] constexpr bool is_dynamic() const
        {
            return this->m_data != this->m_static;
        }

        [[nodiscard]] constexpr std::byte* data()
        {
            return this->m_data;
        }

        [[nodiscard]] constexpr std::byte const* data() const
        {
            return this->m_data;
        }

        [[nodiscard]] constexpr std::span<std::byte> as_span()
        {
            return std::span<std::byte>{this->m_data, this->m_size};
        }

        [[nodiscard]] constexpr std::span<std::byte const> as_span() const
        {
            return std::span<std::byte const>{this->m_data, this->m_size};
        }

        [[nodiscard]] constexpr std::byte& operator[](size_t index)
        {
            assert(index < this->m_size);
            return this->m_data[index];
        }

        [[nodiscard]] constexpr std::byte const& operator[](size_t index) const
        {
            assert(index < this->m_size);
            return this->m_data[index];
        }
    };

    template <size_t StaticCapacity, typename Callback>
    anemone_forceinline bool adapt_memory_buffer(memory_buffer<StaticCapacity>& buffer, Callback&& callback)
    {
        Callback localCallback = std::forward<Callback>(callback);

        size_t requiredSize{};

        if (not localCallback(buffer.as_span(), requiredSize))
        {
            // Failed to adapt the buffer. Callback reported failure.
            buffer.clear();
            return false;
        }

        if (requiredSize <= buffer.capacity())
        {
            // Callback reported success and the buffer is large enough to hold the required size.
            buffer.resize(requiredSize);
            return true;
        }

        size_t bufferLength{};
        do
        {
            bufferLength = requiredSize;

            buffer.resize_for_override(bufferLength);

            if (not localCallback(buffer.as_span(), requiredSize))
            {
                // Failed to adapt the buffer. Callback reported failure.
                buffer.clear();
                return false;
            }
        } while (bufferLength != requiredSize);
        
        assert(bufferLength == requiredSize);
        buffer.resize(bufferLength);
        return true;
    }
}
