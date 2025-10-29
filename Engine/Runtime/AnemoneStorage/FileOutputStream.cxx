#include "AnemoneStorage/FileOutputStream.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

#include <cstring>

namespace Anemone
{
    std::expected<void, Error> FileOutputStream::FlushBuffer()
    {
        if (this->_bufferPosition != 0)
        {
            // Consume internal buffer.
            std::span buffer{this->_buffer.get(), std::exchange(this->_bufferPosition, 0)};

            if (auto r = this->WriteCore(buffer, this->_filePosition))
            {
                this->_filePosition += static_cast<int64_t>(*r);
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        return {};
    }

    std::expected<size_t, Error> FileOutputStream::WriteCore(std::span<std::byte const> buffer, uint64_t position) const
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            if (auto r = this->_handle->WriteAt(buffer, position))
            {
                processed += *r;
                position += *r;
                buffer = buffer.subspan(*r);
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        return processed;
    }

    FileOutputStream::FileOutputStream(Reference<FileHandle> handle)
        : FileOutputStream(std::move(handle), DefaultBufferCapacity)
    {
    }

    FileOutputStream::FileOutputStream(Reference<FileHandle> handle, size_t bufferCapacity)
        : _handle{std::move(handle)}
        , _bufferCapacity{bufferCapacity}
        , _buffer{std::make_unique<std::byte[]>(bufferCapacity)}
    {
    }

    FileOutputStream::~FileOutputStream()
    {
        (void)this->Flush();
    }

    std::expected<size_t, Error> FileOutputStream::Write(std::span<std::byte const> buffer)
    {
        if (buffer.size() >= this->_bufferCapacity)
        {
            // Buffer is too large, flush internal buffer first, then write directly.
            if (auto flushed = this->FlushBuffer())
            {
                if (auto processed = this->WriteCore(buffer, this->_filePosition))
                {
                    this->_filePosition += *processed;
                    return *processed;
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }

        // Buffer is large enough, copy to internal buffer, flush if necessary.
        if ((this->_bufferPosition + buffer.size()) > this->_bufferCapacity)
        {
            // Flush and copy to internal buffer.
            if (auto flushed = this->FlushBuffer())
            {
                std::memcpy(this->_buffer.get(), buffer.data(), buffer.size());
                this->_bufferPosition = buffer.size();
                return buffer.size();
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }

        // Copy to internal buffer.
        std::memcpy(this->_buffer.get() + this->_bufferPosition, buffer.data(), buffer.size());
        this->_bufferPosition += buffer.size();

        AE_ASSERT(this->_bufferPosition <= this->_bufferCapacity);

        return buffer.size();
    }

    std::expected<void, Error> FileOutputStream::Flush()
    {
        if (auto flushed = this->FlushBuffer())
        {
            // Flushed internal writer buffer.
            return this->_handle->Flush();
        }
        else
        {
            return flushed;
        }
    }

    std::expected<void, Error> FileOutputStream::SetPosition(uint64_t value)
    {
        if (auto flushed = this->FlushBuffer())
        {
            this->_filePosition = value;
            return {};
        }
        else
        {
            return flushed;
        }
    }

    std::expected<uint64_t, Error> FileOutputStream::GetPosition() const
    {
        return this->_filePosition;
    }

    std::expected<uint64_t, Error> FileOutputStream::GetLength() const
    {
        return this->_handle->GetLength();
    }
}
