#include "AnemoneRuntime/Storage/BinaryReader.hxx"

namespace Anemone
{
    size_t BinaryReader::FillBuffer()
    {
        this->_bufferLength = 0;
        this->_bufferPosition = 0;

        auto processed = this->_stream->Read(
            std::span{reinterpret_cast<std::byte*>(this->_buffer.get()), this->_bufferCapacity});

        if (processed)
        {
            this->_bufferLength = *processed;
            return this->_bufferLength;
        }

        return 0;
    }

    BinaryReader::BinaryReader(InputStreamRef stream)
        : BinaryReader{std::move(stream), DefaultBufferSize}
    {
    }

    BinaryReader::BinaryReader(InputStreamRef stream, size_t bufferSize)
        : _stream(std::move(stream))
        , _buffer(std::make_unique<char[]>(bufferSize))
        , _bufferCapacity(bufferSize)
    {
    }

    BinaryReader::~BinaryReader() = default;

    bool BinaryReader::EndOfStream()
    {
        if (this->_bufferPosition < this->_bufferLength)
        {
            return false;
        }

        return not this->FillBuffer();
    }

    auto BinaryReader::Read(std::span<std::byte> data) -> std::expected<size_t, Error>
    {
        size_t processed = 0;

        while (not data.empty())
        {
            if (this->_bufferPosition == this->_bufferLength)
            {
                if (this->FillBuffer() == 0)
                {
                    break;
                }
            }

            size_t const remaining = this->_bufferLength - this->_bufferPosition;

            size_t const read = std::min<size_t>(data.size(), remaining);

            std::memcpy(data.data(), this->_buffer.get() + this->_bufferPosition, read);

            this->_bufferPosition += read;

            processed += read;

            data = data.subspan(read);
        }

        if (processed == 0)
        {
            return std::unexpected(Error::EndOfFile);
        }

        return processed;
    }
}
