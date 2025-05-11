#include "AnemoneRuntime/Storage/FileHandleWriter.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Storage
{
    std::expected<void, Status> FileHandleWriter::FlushBuffer()
    {
        if (this->_buffer_position != 0)
        {
            // Consume internal buffer.
            std::span buffer{this->_buffer.get(), std::exchange(this->_buffer_position, 0)};

            while (not buffer.empty())
            {
                if (auto processed = this->_handle.WriteAt(buffer, this->_file_position))
                {
                    this->_file_position += static_cast<int64_t>(*processed);
                    buffer = buffer.subspan(*processed);
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }
        }

        return {};
    }

    FileHandleWriter::FileHandleWriter(FileHandle handle, size_t buffer_capacity)
        : _handle(std::move(handle))
        , _buffer_capacity(buffer_capacity)
        , _buffer(std::make_unique<std::byte[]>(buffer_capacity))
    {
    }

    FileHandleWriter::~FileHandleWriter()
    {
        // Ignore errors?
        (void)this->Flush();
    }

    std::expected<size_t, Status> FileHandleWriter::Write(std::span<std::byte const> buffer)
    {
        if (buffer.size() >= this->_buffer_capacity)
        {
            // Buffer is too large, flush internal buffer first, then write directly.
            if (auto flushed = this->FlushBuffer())
            {
                size_t processed = 0;

                while (not buffer.empty())
                {
                    if (auto written = this->_handle.WriteAt(buffer, this->_file_position))
                    {
                        processed += *written;
                        this->_file_position += static_cast<int64_t>(*written);
                        buffer = buffer.subspan(*written);
                    }
                    else
                    {
                        return std::unexpected(written.error());
                    }
                }

                return processed;
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }

        // Buffer is large enough, copy to internal buffer, flush if necessary.
        if ((this->_buffer_position + buffer.size()) > this->_buffer_capacity)
        {
            // Flush and copy to internal buffer.
            if (auto flushed = this->FlushBuffer())
            {
                std::memcpy(this->_buffer.get(), buffer.data(), buffer.size());
                this->_buffer_position = buffer.size();

                return buffer.size();
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }

        // Copy to internal buffer.
        std::memcpy(this->_buffer.get() + this->_buffer_position, buffer.data(), buffer.size());
        this->_buffer_position += buffer.size();

        AE_ASSERT(this->_buffer_position <= this->_buffer_capacity);

        return buffer.size();
    }

    std::expected<void, Status> FileHandleWriter::Flush()
    {
        if (auto processed = this->FlushBuffer())
        {
            // Flushed internal writer buffer.
            return this->_handle.Flush();
        }
        else
        {
            return processed;
        }
    }

    std::expected<void, Status> FileHandleWriter::SetPosition(int64_t position)
    {
        AE_ASSERT(position >= 0, "Negative file position is not allowed");

        if (0 <= position)
        {
            // New file position is in range of the file
            if (auto flushed = this->FlushBuffer())
            {
                this->_file_position = position;

                return {};
            }
            else
            {
                return flushed;
            }
        }

        return std::unexpected(Status::InvalidArgument);
    }

    std::expected<int64_t, Status> FileHandleWriter::GetPosition() const
    {
        return this->_file_position;
    }
}
