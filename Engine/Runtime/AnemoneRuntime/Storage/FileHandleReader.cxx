#include "AnemoneRuntime/Storage/FileHandleReader.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Storage
{
    size_t FileHandleReader::ReadCore(std::span<std::byte> buffer, uint64_t position) const
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            size_t read = this->_handle->ReadAt(buffer, position);
            if (read == 0)
            {
                // End of file.
                break;
            }

            processed += read;
            position += read;
            buffer = buffer.subspan(read);
        }

        return processed;
    }

    FileHandleReader::FileHandleReader(std::unique_ptr<FileHandle> handle, size_t buffer_capacity)
        : _handle{std::move(handle)}
        , _bufferCapacity{buffer_capacity}
        , _buffer{std::make_unique<std::byte[]>(buffer_capacity)}
    {
        this->_fileSize = this->_handle->GetLength();
    }

    size_t FileHandleReader::Read(std::span<std::byte> buffer)
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

            size_t read = this->ReadCore(buffer, position);
            // Read succeeded. Discard internal buffers and advance file position.
            this->_bufferPosition = 0;
            this->_bufferSize = 0;
            this->_filePosition = position + read;
            return processed + read;
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
                size_t prefetched = this->ReadCore(std::span{this->_buffer.get(), this->_bufferCapacity}, this->_filePosition);
                if (prefetched == 0)
                {
                    // End of file.
                    break;
                }

                this->_bufferSize = prefetched;
                this->_bufferPosition = 0;
            }
            else
            {
                AE_PANIC("Impossible situation");
            }
        }

        return processed;
    }

    void FileHandleReader::Skip(size_t count)
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
    }

    void FileHandleReader::SetPosition(uint64_t position)
    {
        uint64_t const mappedBufferStart = this->_filePosition - this->_bufferPosition;
        uint64_t const mappedBufferEnd = mappedBufferStart + this->_bufferSize;

        AE_ASSERT(mappedBufferStart <= mappedBufferEnd);
        AE_ASSERT(mappedBufferStart <= this->_filePosition);
        AE_ASSERT(this->_filePosition <= mappedBufferEnd);
        AE_ASSERT(mappedBufferEnd <= this->_fileSize);


        if ((mappedBufferStart <= position) and (position <= mappedBufferEnd))
        {
            // Inside buffer.
            this->_bufferPosition = position - mappedBufferStart;
        }
        else if (position <= this->_fileSize)
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
        this->_filePosition = position;
    }

    uint64_t FileHandleReader::GetPosition() const
    {
        return this->_filePosition;
    }
}
