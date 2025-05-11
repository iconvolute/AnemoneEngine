#include "AnemoneRuntime/Storage/MemoryReader.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <algorithm>

namespace Anemone::Storage
{
    MemoryReader::MemoryReader(std::span<std::byte const> buffer)
        : _buffer{buffer}
        , _buffer_position{}
    {
    }

    MemoryReader::~MemoryReader() = default;

    std::expected<size_t, Status> MemoryReader::Read(std::span<std::byte> buffer)
    {
        AE_ASSERT(this->_buffer_position <= this->_buffer.size());

        size_t const remaining = this->_buffer.size() - this->_buffer_position;
        size_t const requested = std::min<size_t>(buffer.size(), remaining);

        if (requested != 0)
        {
            std::copy_n(this->_buffer.data() + this->_buffer_position, requested, buffer.data());

            this->_buffer_position += requested;
        }

        return requested;
    }

    std::expected<void, Status> MemoryReader::SetPosition(int64_t position)
    {
        if ((position < 0) or (static_cast<size_t>(position) > this->_buffer.size()))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        this->_buffer_position = static_cast<size_t>(position);

        return {};
    }

    std::expected<int64_t, Status> MemoryReader::GetPosition() const
    {
        return static_cast<int64_t>(this->_buffer_position);
    }
}
