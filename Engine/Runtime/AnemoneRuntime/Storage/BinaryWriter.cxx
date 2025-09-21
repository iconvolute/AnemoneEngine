#include "AnemoneRuntime/Storage/BinaryWriter.hxx"

namespace Anemone
{
    auto BinaryWriter::FlushBuffer(bool flushStream) -> std::expected<void, Error>
    {
        if (this->_bufferPosition != 0)
        {
            // Consume internal buffer.
            std::span buffer{this->_buffer.get(), std::exchange(this->_bufferPosition, 0)};

            if (auto r = this->_stream->Write(buffer); not r)
            {
                return std::unexpected(r.error());
            }
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

    BinaryWriter::BinaryWriter(OutputStreamRef stream)
        : BinaryWriter{std::move(stream), DefaultBufferSize}
    {
    }

    BinaryWriter::BinaryWriter(OutputStreamRef stream, size_t bufferSize)
        : _stream{std::move(stream)}
        , _buffer{std::make_unique<std::byte[]>(bufferSize)}
        , _bufferCapacity{bufferSize}
    {
    }

    BinaryWriter::~BinaryWriter()
    {
        (void)this->Flush();
    }

    auto BinaryWriter::Write(std::span<std::byte const> data) -> std::expected<size_t, Error>
    {
        size_t processed = 0;

        std::span buffer{this->_buffer.get(), this->_bufferCapacity};

        while (not data.empty())
        {
            if (this->_bufferPosition == this->_bufferCapacity)
            {
                if (auto flush = this->FlushBuffer(false); not flush)
                {
                    return std::unexpected(flush.error());
                }
            }

            size_t const toCopy = std::min<size_t>(this->_bufferCapacity - this->_bufferPosition, data.size());
            std::memcpy(buffer.data() + this->_bufferPosition, data.data(), toCopy);
            this->_bufferPosition += toCopy;
            data = data.subspan(toCopy);
            processed += toCopy;
        }

        return processed;
    }

    auto BinaryWriter::Flush() -> std::expected<void, Error>
    {
        return this->FlushBuffer(true);
    }
}
