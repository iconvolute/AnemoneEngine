#pragma once
#include <memory>
#include <span>
#include <string_view>
#include <cassert>

namespace Anemone::Interop
{
    template <typename CharT, size_t StaticCapacityT>
    class string_buffer final
    {
    private:
        CharT* m_Data{};
        std::unique_ptr<CharT[]> m_Dynamic{};
        size_t m_Capacity{StaticCapacityT};
        size_t m_Size{};
        CharT m_Static[StaticCapacityT];

    public:
        constexpr string_buffer() noexcept
            : m_Data{m_Static}
        {
        }

        constexpr explicit string_buffer(std::basic_string_view<CharT> value)
        {
            this->resize_for_override(value.size() + 1);
            std::copy(value.begin(), value.end(), this->m_Data);
            this->trim(value.size());
        }

        constexpr explicit string_buffer(const char* value, size_t length)
        {
            this->resize_for_override(length + 1);
            std::copy(value, value + length, this->m_Data);
            this->trim(length);
        }

        string_buffer(string_buffer const&) = delete;

        string_buffer(string_buffer&&) = delete;

        string_buffer& operator=(string_buffer const&) = delete;

        string_buffer& operator=(string_buffer&&) = delete;

        ~string_buffer() noexcept = default;

        constexpr void resize_for_override(size_t size)
        {
            if (size < StaticCapacityT)
            {
                // Reset back to static buffer.
                this->m_Dynamic = nullptr;
                this->m_Data = this->m_Static;
                this->m_Capacity = StaticCapacityT;
                this->m_Size = size;
            }
            else if (size > this->m_Capacity)
            {
                size_t const capacity = size;

                this->m_Dynamic = std::make_unique_for_overwrite<CharT[]>(capacity);
                this->m_Data = this->m_Dynamic.get();
                this->m_Capacity = capacity;
                this->m_Size = size;
            }
        }

        constexpr void resize(size_t size)
        {
            // Test if need to copy data from static to dynamic buffer
            if (size > StaticCapacityT)
            {
                size_t const capacity = size;

                this->m_Dynamic = std::make_unique_for_overwrite<CharT[]>(capacity);
                std::copy(this->m_Data, this->m_Data + this->m_Size, this->m_Dynamic.get());
                this->m_Data = this->m_Dynamic.get();
                this->m_Capacity = capacity;
            }
            // Test if need to copy data from dynamic to static buffer
            else if (this->m_Dynamic and (size <= StaticCapacityT))
            {
                std::copy(this->m_Data, this->m_Data + this->m_Size, this->m_Static);
                this->m_Dynamic = nullptr;
                this->m_Data = this->m_Static;
                this->m_Capacity = StaticCapacityT;
            }
            // Trim the buffer
            else
            {
                this->m_Size = size;
            }
        }

        constexpr void trim(size_t size) noexcept
        {
            assert(size <= this->m_Capacity);
            this->m_Data[size] = {};
            this->m_Size = size;
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept
        {
            return this->m_Capacity;
        }

        [[nodiscard]] constexpr size_t size() const noexcept
        {
            return this->m_Size;
        }

        [[nodiscard]] constexpr operator const CharT*() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr CharT* data() noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* data() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const char* c_str() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> as_view() const noexcept
        {
            return std::basic_string_view<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_span() noexcept
        {
            return std::span<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_span() const noexcept
        {
            return std::span<CharT const>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_buffer_span() noexcept
        {
            return std::span<CharT>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_buffer_span() const noexcept
        {
            return std::span<CharT const>{this->m_Data, this->m_Capacity};
        }
    };

    template <typename StringBufferT, typename CallbackT>
    bool adapt_string_buffer(StringBufferT& buffer, CallbackT callback) noexcept
    {
        size_t requiredCapacity{};
        if (not callback(buffer.as_buffer_span(), requiredCapacity))
        {
            buffer.trim(0);
            return false;
        }

        if (requiredCapacity <= buffer.capacity())
        {
            assert(requiredCapacity != 0);
            buffer.trim(requiredCapacity - 1);
            return true;
        }

        size_t bufferLength{};
        do
        {
            bufferLength = requiredCapacity;

            buffer.resize_for_override(bufferLength);

            if (not callback(buffer.as_buffer_span(), requiredCapacity))
            {
                buffer.trim(0);
                return false;
            }
        } while (requiredCapacity > bufferLength);

        assert(requiredCapacity != 0);
        buffer.trim(requiredCapacity - 1);
        return true;
    }
}
