#include "AnemoneRuntime/Storage/FileHandleWriter.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Storage
{
    void FileHandleWriter::FlushBuffer()
    {
        if (this->_bufferPosition != 0)
        {
            // Consume internal buffer.
            std::span buffer{this->_buffer.get(), std::exchange(this->_bufferPosition, 0)};

            size_t r = this->WriteCore(buffer, this->_filePosition);
            this->_filePosition += static_cast<int64_t>(r);
        }
    }

    size_t FileHandleWriter::WriteCore(std::span<std::byte const> buffer, uint64_t position) const
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            size_t r = this->_handle->WriteAt(buffer, position);
            processed += r;
            position += r;
            buffer = buffer.subspan(r);
        }

        return processed;
    }

    FileHandleWriter::FileHandleWriter(std::unique_ptr<FileHandle> handle, size_t buffer_capacity)
        : _handle{std::move(handle)}
        , _bufferCapacity{buffer_capacity}
        , _buffer{std::make_unique<std::byte[]>(buffer_capacity)}
    {
    }

    FileHandleWriter::~FileHandleWriter()
    {
        this->Flush();
    }

    size_t FileHandleWriter::Write(std::span<std::byte const> buffer)
    {
        if (buffer.size() >= this->_bufferCapacity)
        {
            // Buffer is too large, flush internal buffer first, then write directly.
            this->FlushBuffer();

            size_t processed = this->WriteCore(buffer, this->_filePosition);
            this->_filePosition += processed;
            return processed;
        }

        // Buffer is large enough, copy to internal buffer, flush if necessary.
        if ((this->_bufferPosition + buffer.size()) > this->_bufferCapacity)
        {
            // Flush and copy to internal buffer.
            this->FlushBuffer();

            std::memcpy(this->_buffer.get(), buffer.data(), buffer.size());
            this->_bufferPosition = buffer.size();
            return buffer.size();
        }

        // Copy to internal buffer.
        std::memcpy(this->_buffer.get() + this->_bufferPosition, buffer.data(), buffer.size());
        this->_bufferPosition += buffer.size();

        AE_ASSERT(this->_bufferPosition <= this->_bufferCapacity);

        return buffer.size();
    }

    void FileHandleWriter::Flush()
    {
        this->FlushBuffer();

        // Flushed internal writer buffer.
        this->_handle->Flush();
    }

    void FileHandleWriter::SetPosition(uint64_t position)
    {
        if (this->_filePosition != position)
        {
            this->FlushBuffer();
        }
    }

    uint64_t FileHandleWriter::GetPosition() const
    {
        return this->_filePosition;
    }
}
