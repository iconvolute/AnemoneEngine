#include "AnemoneRuntime.Storage/MemoryOutputStream.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

#include <algorithm>

namespace Anemone
{
    MemoryOutputStream::MemoryOutputStream(Reference<MemoryBuffer> buffer)
        : _buffer{std::move(buffer)}
    {
        AE_ASSERT(this->_buffer != nullptr);

        if (auto rc = this->_buffer->EnsureOwned(); not rc)
        {
            AE_PANIC("Cannot ensure that memory buffer is owned");
        }

        if (auto rc = this->_buffer->Resize(0); not rc)
        {
            AE_PANIC("Cannot resize memory buffer");
        }
    }

    MemoryOutputStream::~MemoryOutputStream() = default;

    std::expected<size_t, Error> MemoryOutputStream::Write(std::span<std::byte const> data)
    {
        AE_ASSERT(this->_position <= this->_buffer->GetSize());

        if (not data.empty())
        {
            // Make sure that memory buffer is large enough.
            size_t const newSize = this->_position + data.size();

            if (auto rc = this->_buffer->Resize(newSize))
            {
                // Resized successfully. Copy data into buffer.
                std::copy_n(data.data(), data.size(), this->_buffer->GetData() + this->_position);
                return data.size();
            }
            else
            {
                // Cannot resize buffer.
                return std::unexpected(rc.error());
            }
        }

        return 0;
    }

    std::expected<void, Error> MemoryOutputStream::Flush()
    {
        return {};
    }

    std::expected<void, Error> MemoryOutputStream::SetPosition(uint64_t position)
    {
        AE_ASSERT(this->_position <= this->_buffer->GetSize());

        this->_position = position;

        if (this->_position > this->_buffer->GetSize())
        {
            if (auto rc = this->_buffer->Resize(this->_position); not rc)
            {
                return std::unexpected(rc.error());
            }
        }

        AE_ASSERT(this->_position <= this->_buffer->GetSize());

        return {};
    }

    std::expected<uint64_t, Error> MemoryOutputStream::GetPosition() const
    {
        AE_ASSERT(this->_position <= this->_buffer->GetSize());
        return this->_position;
    }

    std::expected<uint64_t, Error> MemoryOutputStream::GetLength() const
    {
        return this->_buffer->GetSize();
    }
}
