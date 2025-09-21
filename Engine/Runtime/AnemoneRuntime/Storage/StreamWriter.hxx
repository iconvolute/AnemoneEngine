#pragma once
#include "AnemoneRuntime/Storage/TextWriter.hxx"
#include "AnemoneRuntime/Storage/OutputStream.hxx"

#include <memory>

namespace Anemone
{
    class StreamWriter : public TextWriter
    {
        static constexpr size_t DefaultBufferSize = 1024uz;

    private:
        OutputStreamRef _stream{};

        std::unique_ptr<char[]> _buffer{};
        size_t _bufferCapacity{DefaultBufferSize};
        size_t _bufferPosition{0};
        bool _autoFlush{false};

    protected:
        std::expected<void, Error> WriteCore(std::string_view value, bool newline) override;

        std::expected<void, Error> FlushBuffer(bool flushStream);

    public:
        explicit StreamWriter(OutputStreamRef stream);

        StreamWriter(OutputStreamRef stream, size_t bufferSize);

        StreamWriter(StreamWriter const&) = delete;

        StreamWriter(StreamWriter&&) = delete;

        ~StreamWriter() override;

        StreamWriter& operator=(StreamWriter const&) = delete;

        StreamWriter& operator=(StreamWriter&&) = delete;

    public:
        [[nodiscard]] bool GetAutoFlush() const;

        void SetAutoFlush(bool value);

        std::expected<void, Error> Flush() override;
    };
}
