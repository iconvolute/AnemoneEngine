#include "AnemoneRuntime.Storage/StreamReader.hxx"

#include <cstring>

namespace Anemone
{
    size_t StreamReader::FillBuffer()
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

    StreamReader::StreamReader(InputStreamRef stream)
        : StreamReader{std::move(stream), DefaultBufferSize}
    {
    }

    StreamReader::StreamReader(InputStreamRef stream, size_t bufferSize)
        : _stream(std::move(stream))
        , _buffer(std::make_unique<char[]>(bufferSize))
        , _bufferCapacity(bufferSize)
    {
    }

    StreamReader::~StreamReader() = default;

    bool StreamReader::EndOfStream()
    {
        if (this->_bufferPosition < this->_bufferLength)
        {
            return false;
        }

        return not this->FillBuffer();
    }

    std::expected<size_t, Error> StreamReader::Read(std::span<char> buffer)
    {
        size_t processed = 0;

        while (not buffer.empty())
        {
            if (this->_bufferPosition == this->_bufferLength)
            {
                if (this->FillBuffer() == 0)
                {
                    break;
                }
            }

            size_t const remaining = this->_bufferLength - this->_bufferPosition;

            size_t const read = std::min<size_t>(buffer.size(), remaining);

            std::memcpy(buffer.data(), this->_buffer.get() + this->_bufferPosition, read);

            this->_bufferPosition += read;

            processed += read;

            buffer = buffer.subspan(read);
        }

        if (processed == 0)
        {
            return std::unexpected(Error::EndOfFile);
        }

        return processed;
    }

    std::expected<std::string, Error> StreamReader::ReadLine()
    {
        if (this->_bufferPosition == this->_bufferLength)
        {
            if (this->FillBuffer() == 0)
            {
                return std::unexpected(Error::EndOfFile);
            }
        }

        std::string result{};

        do
        {
            std::string_view buffer{this->_buffer.get() + this->_bufferPosition, this->_bufferLength - this->_bufferPosition};

            size_t lineEnd = buffer.find_first_of("\r\n");
            if (lineEnd != std::string_view::npos)
            {
                // Found line ending - append to result string.
                result.append(buffer.substr(0, lineEnd));
                char const matched = buffer[lineEnd];

                this->_bufferPosition += lineEnd + 1;

                if (matched == '\r')
                {
                    if ((this->_bufferPosition < this->_bufferLength) or (this->FillBuffer() != 0))
                    {
                        if (this->_buffer.get()[this->_bufferPosition] == '\n')
                        {
                            ++this->_bufferPosition;
                        }
                    }
                }

                break;
            }

            // No line ending - append and refill buffer.
            result.append(buffer);

        } while (this->FillBuffer() != 0);

        return result;
    }

    std::expected<std::string, Error> StreamReader::ReadToEnd()
    {
        std::string result{};

        do
        {
            result.append(this->_buffer.get() + this->_bufferPosition, this->_bufferLength - this->_bufferPosition);
            this->_bufferPosition = this->_bufferLength;

            if (this->FillBuffer() == 0)
            {
                this->_bufferLength = 0;
                this->_bufferPosition = 0;

                return std::unexpected(Error::EndOfFile);
            }
        } while (this->_bufferLength != 0);

        return result;
    }

    std::expected<char, Error> StreamReader::Read()
    {
        if (this->_bufferPosition == this->_bufferLength)
        {
            if (this->FillBuffer() == 0)
            {
                return std::unexpected(Error::EndOfFile);
            }
        }

        AE_ASSERT(this->_bufferPosition < this->_bufferLength);
        return this->_buffer[this->_bufferPosition++];
    }

    std::expected<char, Error> StreamReader::Peek()
    {
        if (this->_bufferPosition == this->_bufferLength)
        {
            if (this->FillBuffer() == 0)
            {
                return std::unexpected(Error::EndOfFile);
            }
        }

        AE_ASSERT(this->_bufferPosition < this->_bufferLength);
        return this->_buffer[this->_bufferPosition];
    }
}
