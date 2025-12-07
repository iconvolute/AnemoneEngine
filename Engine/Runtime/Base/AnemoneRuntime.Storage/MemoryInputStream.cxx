#include "AnemoneRuntime.Storage/MemoryInputStream.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

#include <algorithm>

namespace Anemone
{
    MemoryInputStream::MemoryInputStream(Reference<MemoryBuffer> buffer)
        : _buffer{std::move(buffer)}
        , _position{}
    {
    }

    MemoryInputStream::~MemoryInputStream() = default;

    std::expected<size_t, Error> MemoryInputStream::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_position <= this->_buffer->GetSize());

        size_t const remaining = this->_buffer->GetSize() - this->_position;
        size_t const requested = std::min<size_t>(buffer.size(), remaining);

        if (requested != 0)
        {
            std::copy_n(this->_buffer->GetData() + this->_position, requested, buffer.data());

            this->_position += requested;
        }

        return requested;
    }

    std::expected<void, Error> MemoryInputStream::SetPosition(uint64_t position)
    {
        position = std::min<uint64_t>(position, this->_buffer->GetSize());

        this->_position = position;
        return {};
    }

    std::expected<uint64_t, Error> MemoryInputStream::GetPosition() const
    {
        return this->_position;
    }

    std::expected<uint64_t, Error> MemoryInputStream::GetLength() const
    {
        return this->_buffer->GetSize();
    }
}
