#include "AnemoneRuntime.Base/MemoryBuffer.hxx"

namespace Anemone
{
    std::expected<Reference<MemoryBuffer>, Error> MemoryBuffer::CreateView(std::span<std::byte> buffer)
    {
        return Reference{new (std::nothrow) MemoryBuffer(buffer.data(), buffer.size(), buffer.size(), false)};
    }

    std::expected<Reference<MemoryBuffer>, Error> MemoryBuffer::Create(std::span<std::byte const> content)
    {
        std::byte* data = nullptr;

        if (not content.empty())
        {
            data = static_cast<std::byte*>(::operator new(content.size(), std::nothrow));

            if (not data)
            {
                return std::unexpected(Error::NotEnoughMemory);
            }

            std::memcpy(data, content.data(), content.size());
        }

        return Reference{new (std::nothrow) MemoryBuffer(data, content.size(), content.size(), true)};
    }

    std::expected<Reference<MemoryBuffer>, Error> MemoryBuffer::Create(size_t size)
    {
        std::byte* data = nullptr;

        if (size != 0)
        {
            data = static_cast<std::byte*>(::operator new(size, std::nothrow));

            if (not data)
            {
                return std::unexpected(Error::NotEnoughMemory);
            }

            std::memset(data, 0, size);
        }

        return Reference{new (std::nothrow) MemoryBuffer(data, size, size, true)};
    }

    std::size_t MemoryBuffer::CalculateGrowth(std::size_t current, std::size_t requested)
    {
        if (current == 0)
        {
            return requested;
        }

        std::size_t const grown = current + current / 2; // 1.5x

        return std::max(grown, requested);
    }

    std::expected<void, Error> MemoryBuffer::Reserve(std::size_t capacity)
    {
        if (not this->_owned)
        {
            return std::unexpected(Error::NotSupported);
        }

        if (capacity <= this->_capacity)
        {
            return {};
        }

        std::size_t const newCapacity = CalculateGrowth(this->_capacity, capacity);

        std::byte* data = static_cast<std::byte*>(::operator new(newCapacity, std::nothrow));

        if (not data)
        {
            return std::unexpected(Error::NotEnoughMemory);
        }

        if (this->_size != 0)
        {
            std::memcpy(data, this->_data, this->_size);
        }

        if (newCapacity > this->_size)
        {
            std::memset(data + this->_size, 0, newCapacity - this->_size);
        }

        ::operator delete(this->_data);
        this->_data = data;
        this->_capacity = newCapacity;

        return {};
    }

    std::expected<void, Error> MemoryBuffer::Resize(std::size_t size)
    {
        if (not this->_owned)
        {
            return std::unexpected(Error::NotSupported);
        }

        if (size <= this->_capacity)
        {
            if (size > this->_size)
            {
                std::memset(this->_data + this->_size, 0, size - this->_size);
            }

            this->_size = size;

            return {};
        }

        auto reserveResult = this->Reserve(size);

        if (not reserveResult)
        {
            return reserveResult;
        }

        this->_size = size;

        return {};
    }

    std::expected<void, Error> MemoryBuffer::Clear()
    {
        if (not this->_owned)
        {
            return std::unexpected(Error::NotSupported);
        }

        this->_size = 0;
        return {};
    }

    std::expected<void, Error> MemoryBuffer::ShrinkToFit()
    {
        if (!this->_owned)
        {
            return std::unexpected(Error::NotSupported);
        }

        if (this->_capacity == this->_size)
        {
            return {};
        }

        if (this->_size == 0)
        {
            ::operator delete(this->_data);
            this->_data = nullptr;
            this->_capacity = 0;

            return {};
        }

        std::byte* data = static_cast<std::byte*>(::operator new(this->_size, std::nothrow));

        if (!data)
        {
            return std::unexpected(Error::NotEnoughMemory);
        }

        std::memcpy(data, this->_data, this->_size);
        ::operator delete(this->_data);
        this->_data = data;
        this->_capacity = this->_size;

        return {};
    }

    std::expected<void, Error> MemoryBuffer::EnsureOwned()
    {
        if (this->_owned)
        {
            return {};
        }

        std::byte* data = static_cast<std::byte*>(::operator new(this->_size, std::nothrow));

        if (not data)
        {
            return std::unexpected(Error::NotEnoughMemory);
        }

        std::memcpy(data, this->_data, this->_size);
        this->_data = data;
        this->_owned = true;
        this->_capacity = this->_size;

        return {};
    }

    std::expected<Reference<MemoryBuffer>, Error> MemoryBuffer::Clone() const
    {
        return Create(this->GetView());
    }
}
