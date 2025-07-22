#include "AnemoneRuntime/Storage/MemoryReader.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <algorithm>

namespace Anemone::Storage
{
    MemoryReader::MemoryReader(std::span<std::byte const> buffer)
        : _buffer{buffer}
        , _position{}
    {
    }

    MemoryReader::~MemoryReader() = default;

    size_t MemoryReader::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_position <= this->_buffer.size());

        size_t const remaining = this->_buffer.size() - this->_position;
        size_t const requested = std::min<size_t>(buffer.size(), remaining);

        if (requested != 0)
        {
            std::copy_n(this->_buffer.data() + this->_position, requested, buffer.data());

            this->_position += requested;
        }

        return requested;
    }

    void MemoryReader::SetPosition(uint64_t position)
    {
        position = std::min<uint64_t>(position, this->_buffer.size());

        this->_position = position;
    }

    uint64_t MemoryReader::GetPosition() const
    {
        return this->_position;
    }
}
