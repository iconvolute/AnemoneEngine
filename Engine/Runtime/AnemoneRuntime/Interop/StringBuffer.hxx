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
        size_t m_Size{};
        std::unique_ptr<CharT[]> m_Dynamic{};
        size_t m_Capacity{StaticCapacityT};
        CharT m_Static[StaticCapacityT + 1];

    public:
        constexpr string_buffer()
            : m_Data{m_Static}
        {
        }

        constexpr explicit string_buffer(std::basic_string_view<CharT> value)
            : m_Data{m_Static}
        {
            this->resize_for_override(value.size());
            std::copy(value.begin(), value.end(), this->m_Data);
            this->trim(value.size());
        }

        constexpr explicit string_buffer(const char* value, size_t length)
            : m_Data{m_Static}
        {
            this->resize_for_override(length);
            std::copy(value, value + length, this->m_Data);
            this->trim(length);
        }

        string_buffer(string_buffer const&) = delete;

        string_buffer(string_buffer&&) = delete;

        string_buffer& operator=(string_buffer const&) = delete;

        string_buffer& operator=(string_buffer&&) = delete;

        ~string_buffer() = default;

        constexpr void resize_for_override(size_t size)
        {
            if (size == this->m_Size)
            {
                // Do nothing.
                return;
            }

            if (size < StaticCapacityT)
            {
                // Reset back to static buffer.
                this->m_Dynamic = nullptr;
                this->m_Data = this->m_Static;
                this->m_Capacity = StaticCapacityT;
            }
            else if (size > this->m_Capacity)
            {
                size_t const capacity = size;

                this->m_Dynamic = std::make_unique_for_overwrite<CharT[]>(capacity + 1uz);
                this->m_Data = this->m_Dynamic.get();
                this->m_Capacity = capacity;
            }

            this->m_Size = size;
            this->m_Data[this->m_Size] = {};
        }

        template <typename Callback = size_t(CharT* data, size_t size)>
        constexpr void resize_and_override(size_t size, Callback&& callback)
        {
            this->resize_for_override(size);
            size_t const result = std::forward<Callback>(callback)(this->m_Data, this->m_Size);
            this->trim(result);
        }

        constexpr void resize(size_t size, CharT fill = CharT{})
        {
            assert(this->m_Capacity >= StaticCapacityT);
            assert(this->m_Size <= this->m_Capacity);

            if (size != this->m_Size)
            {
                if (size <= this->m_Capacity)
                {
                    // Allocated buffer can hold the new size.
                    if (size > this->m_Size)
                    {
                        // Fill rest of the buffer with fill character.
                        std::fill(this->m_Data + this->m_Size, this->m_Data + size, fill);
                    }
                }
                else
                {
                    // If we are here, we need to allocate a new buffer.
                    size_t const capacity = size;

                    std::unique_ptr<CharT[]> newBuffer = std::make_unique_for_overwrite<CharT[]>(capacity + 1uz);
                    std::copy(this->m_Data, this->m_Data + this->m_Size, newBuffer.get());
                    std::fill(newBuffer.get() + this->m_Size, newBuffer.get() + size, fill);

                    this->m_Data = newBuffer.get();
                    this->m_Dynamic = std::move(newBuffer);
                    this->m_Capacity = capacity;
                }

                // Trim the buffer.
                this->m_Data[size] = CharT{};
                this->m_Size = size;
            }
        }

        constexpr void trim(size_t size)
        {
            assert(size <= this->m_Capacity);
            this->m_Data[size] = {};
            this->m_Size = size;
        }

        constexpr void clear()
        {
            this->m_Data[0uz] = CharT{};
            this->m_Size = 0uz;
        }

        [[nodiscard]] constexpr size_t capacity() const
        {
            return this->m_Capacity;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return this->m_Size;
        }

        [[nodiscard]] constexpr operator const CharT*() const
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr CharT* data()
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* data() const
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* c_str() const
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr bool is_dynamic() const
        {
            return this->m_Data != this->m_Static;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> as_view() const
        {
            return std::basic_string_view<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_span()
        {
            return std::span<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_span() const
        {
            return std::span<CharT const>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_buffer_span()
        {
            return std::span<CharT>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_buffer_span() const
        {
            return std::span<CharT const>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr CharT& operator[](size_t index)
        {
            assert(index < this->m_Size);
            return this->m_Data[index];
        }

        [[nodiscard]] constexpr CharT const& operator[](size_t index) const
        {
            assert(index < this->m_Size);
            return this->m_Data[index];
        }
    };

    template <typename StringBuffer, typename Callback>
    bool adapt_string_buffer(StringBuffer& buffer, Callback&& callback)
    {
        Callback localCallback = std::forward<Callback>(callback);

        size_t requiredSize{};
        if (not localCallback(buffer.as_buffer_span(), requiredSize))
        {
            buffer.trim(0);
            return false;
        }

        if (requiredSize <= buffer.capacity())
        {
            assert(requiredSize != 0);
            buffer.trim(requiredSize);
            return true;
        }

        size_t bufferLength{};
        do
        {
            bufferLength = requiredSize;

            buffer.resize_for_override(bufferLength);

            if (not localCallback(buffer.as_buffer_span(), requiredSize))
            {
                buffer.trim(0);
                return false;
            }
        } while (requiredSize > bufferLength);

        assert(requiredSize != 0);
        buffer.trim(requiredSize);
        return true;
    }
}
