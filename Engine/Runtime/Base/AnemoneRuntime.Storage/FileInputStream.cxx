#include "AnemoneRuntime.Storage/FileInputStream.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

#include <cstring>

namespace Anemone
{
    std::expected<size_t, Error> FileInputStream::ReadCore(std::span<std::byte> buffer, uint64_t position) const
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            if (auto r = this->_handle->ReadAt(buffer, position))
            {
                if (*r == 0)
                {
                    break;
                }

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

    FileInputStream::FileInputStream(Reference<FileHandle> handle)
        : FileInputStream(std::move(handle), DefaultBufferCapacity)
    {
    }

    FileInputStream::FileInputStream(Reference<FileHandle> handle, size_t bufferCapacity)
        : _handle{std::move(handle)}
        , _bufferCapacity{bufferCapacity}
        , _buffer{std::make_unique<std::byte[]>(bufferCapacity)}
    {
        this->_fileSize = this->_handle->GetLength().value_or(0);
    }

    FileInputStream::~FileInputStream() = default;

    std::expected<size_t, Error> FileInputStream::Read(std::span<std::byte> buffer)
    {
        // If output buffer is large enough, do read directly there.
        if (buffer.size() > this->_bufferCapacity)
        {
            // Start transaction.
            size_t const processed = this->_bufferSize - this->_bufferPosition;
            uint64_t const position = this->_filePosition + processed;

            if (processed != 0)
            {
                // Consume data from buffer.
                std::memcpy(buffer.data(), this->_buffer.get() + this->_bufferPosition, processed);

                // Update position just after buffer.
                buffer = buffer.subspan(processed);
            }

            if (auto r = this->ReadCore(buffer, position))
            {
                // Read succeeded. Discard internal buffers and advance file position.
                this->_bufferPosition = 0;
                this->_bufferSize = 0;
                this->_filePosition = position + *r;
                return processed + *r;
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        size_t processed = 0;

        while (not buffer.empty())
        {
            if (size_t const remaining = this->_bufferSize - this->_bufferPosition)
            {
                // Copy data from internal buffer.
                size_t const read = std::min<size_t>(buffer.size(), remaining);
                std::memcpy(buffer.data(), this->_buffer.get() + this->_bufferPosition, read);

                // Update position just after buffer.
                this->_bufferPosition += read;
                this->_filePosition += read;
                processed += read;
                buffer = buffer.subspan(read);
            }
            else if (this->_bufferPosition == this->_bufferSize)
            {
                // Buffer is empty, try to read more data.
                if (auto r = this->ReadCore(std::span{this->_buffer.get(), this->_bufferCapacity}, this->_filePosition))
                {
                    if (*r == 0)
                    {
                        break;
                    }

                    this->_bufferSize = *r;
                    this->_bufferPosition = 0;
                }
                else
                {
                    return std::unexpected(r.error());
                }
            }
            else
            {
                AE_PANIC("Impossible situation");
            }
        }

        return processed;
    }

    std::expected<void, Error> FileInputStream::Skip(size_t count)
    {
        if (count != 0)
        {
            size_t const remaining = this->_bufferSize - this->_bufferPosition;

            if (count < remaining)
            {
                // Inside buffer.
                this->_bufferPosition += count;
            }
            else
            {
                // Outside buffer.
                this->_bufferSize = 0;
                this->_bufferPosition = 0;
            }

            this->_filePosition += count;
        }

        return {};
    }

    std::expected<void, Error> FileInputStream::SetPosition(uint64_t value)
    {
        uint64_t const mappedBufferStart = this->_filePosition - this->_bufferPosition;
        uint64_t const mappedBufferEnd = mappedBufferStart + this->_bufferSize;

        AE_ASSERT(mappedBufferStart <= mappedBufferEnd);
        AE_ASSERT(mappedBufferStart <= this->_filePosition);
        AE_ASSERT(this->_filePosition <= mappedBufferEnd);
        AE_ASSERT(mappedBufferEnd <= this->_fileSize);


        if ((mappedBufferStart <= value) and (value <= mappedBufferEnd))
        {
            // Inside buffer.
            this->_bufferPosition = value - mappedBufferStart;
        }
        else if (value <= this->_fileSize)
        {
            // Outside buffer.
            this->_bufferSize = 0;
            this->_bufferPosition = 0;
        }
        else
        {
            AE_PANIC("Invalid position. Implement truncation at the end of the file.");
        }

        // Update file position.
        this->_filePosition = value;
        return {};
    }

    std::expected<uint64_t, Error> FileInputStream::GetPosition() const
    {
        return this->_filePosition;
    }

    std::expected<uint64_t, Error> FileInputStream::GetLength() const
    {
        return this->_fileSize;
    }
}
