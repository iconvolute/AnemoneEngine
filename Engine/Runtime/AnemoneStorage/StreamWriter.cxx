#include "AnemoneStorage/StreamWriter.hxx"

#include <cstring>

namespace Anemone
{
    std::expected<void, Error> StreamWriter::WriteCore(std::string_view value, bool newline)
    {
        std::span buffer{this->_buffer.get(), this->_bufferCapacity};

        while (not value.empty())
        {
            if (this->_bufferPosition == this->_bufferCapacity)
            {
                if (auto flush = this->FlushBuffer(false); not flush)
                {
                    return std::unexpected(flush.error());
                }
            }

            size_t const toCopy = std::min<size_t>(this->_bufferCapacity - this->_bufferPosition, value.size());
            std::memcpy(buffer.data() + this->_bufferPosition, value.data(), toCopy);
            this->_bufferPosition += toCopy;
            value.remove_prefix(toCopy);
        }

        if (newline)
        {
            if (this->_bufferPosition == this->_bufferCapacity)
            {
                if (auto flush = this->FlushBuffer(false); not flush)
                {
                    return std::unexpected(flush.error());
                }
            }

            buffer[this->_bufferPosition] = '\n';
            ++this->_bufferPosition;
        }

        if (this->_autoFlush)
        {
            if (auto flush = this->FlushBuffer(true); not flush)
            {
                return std::unexpected(flush.error());
            }
        }

        return {};
    }

    std::expected<void, Error> StreamWriter::FlushBuffer(bool flushStream)
    {
        if (this->_bufferPosition != 0)
        {
            if (auto write = this->_stream->Write(std::as_bytes(std::span{this->_buffer.get(), this->_bufferPosition})); not write)
            {
                return std::unexpected(write.error());
            }

            this->_bufferPosition = 0;
        }

        if (flushStream)
        {
            if (auto flush = this->_stream->Flush(); not flush)
            {
                return std::unexpected(flush.error());
            }
        }

        return {};
    }

    StreamWriter::StreamWriter(OutputStreamRef stream)
        : StreamWriter(std::move(stream), DefaultBufferSize)
    {
    }

    StreamWriter::StreamWriter(OutputStreamRef stream, size_t bufferSize)
        : _stream{std::move(stream)}
        , _buffer{std::make_unique<char[]>(bufferSize)}
        , _bufferCapacity{bufferSize}
    {
    }

    StreamWriter::~StreamWriter()
    {
        (void)this->FlushBuffer(true);
    }

    bool StreamWriter::GetAutoFlush() const
    {
        return this->_autoFlush;
    }

    void StreamWriter::SetAutoFlush(bool value)
    {
        this->_autoFlush = value;

        (void)this->FlushBuffer(true);
    }

    std::expected<void, Error> StreamWriter::Flush()
    {
        return this->FlushBuffer(true);
    }
}
