#include "AnemoneRuntime/Storage/FileHandleReader.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Storage
{
    std::expected<size_t, ErrorCode> FileHandleReader::ReadCore(std::span<std::byte> buffer, int64_t position)
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            if (auto r = this->_handle.ReadAt(buffer, position))
            {
                if (*r == 0)
                {
                    // End of file.
                    break;
                }

                processed += *r;
                position += static_cast<int64_t>(*r);
                buffer = buffer.subspan(*r);
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        return processed;
    }

    FileHandleReader::FileHandleReader(FileHandle handle, size_t buffer_capacity)
        : _handle(std::move(handle))
        , _buffer_capacity(buffer_capacity)
        , _buffer(std::make_unique<std::byte[]>(buffer_capacity))
    {
        this->_file_size = this->_handle.GetLength().value_or(0);
    }

    std::expected<size_t, ErrorCode> FileHandleReader::Read(std::span<std::byte> buffer)
    {
        if (buffer.size() > this->_buffer_capacity)
        {
            // Start transaction.
            size_t processed = this->_buffer_size - this->_buffer_position;
            int64_t position = this->_file_position + static_cast<int64_t>(processed);

            if (processed != 0)
            {
                // Consume data from buffer.
                std::memcpy(buffer.data(), this->_buffer.get() + this->_buffer_position, processed);

                // Update position just after buffer.
                buffer = buffer.subspan(processed);
            }

            if (auto read = this->ReadCore(buffer, position))
            {
                // Read succeeded. Discard internal buffers and advance file position.
                this->_buffer_position = 0;
                this->_buffer_size = 0;
                this->_file_position = position + static_cast<int64_t>(*read);
                return processed + *read;
            }
            else
            {
                // Failed to read data. Discard whole transaction.
                return std::unexpected(read.error());
            }
        }

        size_t processed = 0;

        while (not buffer.empty())
        {
            if (size_t const remaining = this->_buffer_size - this->_buffer_position)
            {
                // Copy data from internal buffer.
                size_t const read = std::min<size_t>(buffer.size(), remaining);
                std::memcpy(buffer.data(), this->_buffer.get() + this->_buffer_position, read);

                // Update position just after buffer.
                this->_buffer_position += read;
                this->_file_position += static_cast<int64_t>(read);
                processed += read;
                buffer = buffer.subspan(read);
            }
            else if (this->_buffer_position == this->_buffer_size)
            {
                // Buffer is empty, try to read more data.
                if (auto prefetched = this->ReadCore(std::span{this->_buffer.get(), this->_buffer_capacity}, this->_file_position))
                {
                    if (*prefetched == 0)
                    {
                        // End of file.
                        break;
                    }

                    this->_buffer_size = *prefetched;
                    this->_buffer_position = 0;
                }
                else
                {
                    // Failed to prefetch data.
                    return std::unexpected(prefetched.error());
                }
            }
            else
            {
                AE_PANIC("Impossible situation");
            }
        }

        return processed;
    }

    std::expected<void, ErrorCode> FileHandleReader::Skip(size_t count)
    {
        if (count != 0)
        {
            size_t const remaining = this->_buffer_size - this->_buffer_position;

            if (count < remaining)
            {
                // Inside buffer.
                this->_buffer_position += count;
            }
            else
            {
                // Outside buffer.
                this->_buffer_size = 0;
                this->_buffer_position = 0;
            }

            this->_file_position += static_cast<int64_t>(count);
        }

        return {};
    }

    std::expected<void, ErrorCode> FileHandleReader::SetPosition(int64_t position)
    {
        int64_t const mappedBufferStart = this->_file_position - static_cast<int64_t>(this->_buffer_position);
        int64_t const mappedBufferEnd = mappedBufferStart + static_cast<int64_t>(this->_buffer_size);

        AE_ASSERT(mappedBufferStart <= mappedBufferEnd);
        AE_ASSERT(0 <= mappedBufferStart);
        AE_ASSERT(mappedBufferStart <= this->_file_position);
        AE_ASSERT(this->_file_position <= mappedBufferEnd);
        AE_ASSERT(mappedBufferEnd <= this->_file_size);


        if ((mappedBufferStart <= position) and (position <= mappedBufferEnd))
        {
            // Inside buffer.
            this->_buffer_position = static_cast<size_t>(position - mappedBufferStart);
        }
        else if ((0 <= position) and (position <= this->_file_size))
        {
            // Outside buffer.
            this->_buffer_size = 0;
            this->_buffer_position = 0;
        }
        else
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        // Update file position.
        this->_file_position = position;

        return {};
    }

    std::expected<int64_t, ErrorCode> FileHandleReader::GetPosition() const
    {
        return this->_file_position;
    }
}
