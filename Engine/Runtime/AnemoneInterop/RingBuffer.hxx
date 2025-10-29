#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

#include <memory>
#include <span>
#include <cassert>

namespace Anemone::Interop
{
    class ring_buffer_view final
    {
    private:
        std::span<std::byte> _buffer;
        std::byte* _end;
        std::byte* _head;
        std::byte* _tail;

    public:
        ring_buffer_view(std::span<std::byte> buffer)
            : _buffer{buffer}
            , _end{buffer.data() + buffer.size()}
            , _head{buffer.data()}
            , _tail{buffer.data()}
        {
        }

        ring_buffer_view(ring_buffer_view const&) = delete;

        ring_buffer_view(ring_buffer_view&&) = delete;

        ring_buffer_view& operator=(ring_buffer_view const&) = delete;

        ring_buffer_view& operator=(ring_buffer_view&&) = delete;

        ~ring_buffer_view() = default;

    public:
        size_t available_space() const
        {
            std::byte* afterTail = ((this->_tail + 1uz) == this->_end)
                ? this->_buffer.data()
                : (this->_tail + 1uz);

            if (afterTail == this->_head)
            {
                return 0uz;
            }

            if (this->_tail == this->_head)
            {
                return this->_buffer.size() - 1uz;
            }

            if (this->_tail > this->_head)
            {
                return this->_buffer.size() - (this->_tail - this->_head) - 1uz;
            }

            return (this->_head - this->_tail) - 1uz;
        }

        size_t available_data() const
        {
            size_t const space = this->available_space();
            return this->_buffer.size() - space - 1uz;
        }

        bool write(std::span<std::byte const> data)
        {
            AE_ASSERT(this->_tail < this->_end);

            size_t toCopy = data.size();

            if (toCopy == 0uz)
            {
                return false;
            }

            size_t const availableSpace = this->available_space();

            if (availableSpace < data.size())
            {
                return false;
            }

            if ((this->_tail + toCopy) > this->_end)
            {
                // Wrap around the buffer.
                size_t const spaceToEnd = this->_end - this->_tail;

                // Copy from tail to end.
                memcpy(this->_tail, data.data(), spaceToEnd);

                toCopy -= spaceToEnd;

                // Copy from beginning of the buffer.
                memcpy(this->_buffer.data(), data.data() + spaceToEnd, toCopy);

                this->_tail = this->_buffer.data() + toCopy;
            }
            else
            {
                // Everything fits between tail and end.
                memcpy(this->_tail, data.data(), toCopy);

                this->_tail += toCopy;

                if (this->_tail == this->_end)
                {
                    this->_tail = this->_buffer.data();
                }
            }

            AE_ASSERT(this->_tail < this->_end);

            return true;
        }

        bool read(std::span<std::byte> data)
        {
            AE_ASSERT(this->_head < this->_end);

            size_t toCopy = data.size();

            if (toCopy == 0uz)
            {
                return false;
            }

            size_t const available = this->available_data();

            if (available < data.size())
            {
                // Not enough data to read.
                return false;
            }

            if ((this->_head + data.size()) > this->_end)
            {
                // Wrap around the buffer.
                size_t toEnd = this->_end - this->_head;

                memcpy(data.data(), this->_head, toEnd);

                toCopy -= toEnd;

                memcpy(data.data() + toEnd, this->_buffer.data(), toCopy);

                this->_head = this->_buffer.data() + toCopy;
            }
            else
            {
                // Everything fits between head and end.
                memcpy(data.data(), this->_head, toCopy);

                this->_head += toCopy;

                if (this->_head == this->_end)
                {
                    this->_head = this->_buffer.data();
                }
            }

            AE_ASSERT(this->_head < this->_end);
            return true;
        }
    };
}
