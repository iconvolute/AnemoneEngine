#pragma once
#include "AnemoneRuntime/Storage/TextReader.hxx"
#include "AnemoneRuntime/Storage/InputStream.hxx"

#include <memory>
#include <span>
#include <string_view>

namespace Anemone
{
    class RUNTIME_API StreamReader : public TextReader
    {
        static constexpr size_t DefaultBufferSize = 1024uz;

    private:
        InputStreamRef _stream{};
        std::unique_ptr<char[]> _buffer{};
        size_t _bufferLength{0};
        size_t _bufferCapacity{DefaultBufferSize};
        size_t _bufferPosition{0};

    private:
        [[nodiscard]] size_t FillBuffer();

    public:
        explicit StreamReader(InputStreamRef stream);

        StreamReader(InputStreamRef stream, size_t bufferSize);

        StreamReader(StreamReader const&) = delete;

        StreamReader(StreamReader&&) = delete;

        ~StreamReader() override;

        StreamReader& operator=(StreamReader const&) = delete;

        StreamReader& operator=(StreamReader&&) = delete;

    public:
        bool EndOfStream();

        std::expected<size_t, Error> Read(std::span<char> buffer) override;

        std::expected<std::string, Error> ReadLine() override;

        std::expected<std::string, Error> ReadToEnd() override;

        std::expected<char, Error> Read() override;

        std::expected<char, Error> Peek() override;
    };
}
