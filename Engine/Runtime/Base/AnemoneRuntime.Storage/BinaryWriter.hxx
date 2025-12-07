#pragma once
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.Storage/OutputStream.hxx"

#include <memory>

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API BinaryWriter final : public ReferenceCounted<BinaryWriter>
    {
        static constexpr size_t DefaultBufferSize = 1024uz;

    private:
        OutputStreamRef _stream{};

        std::unique_ptr<std::byte[]> _buffer{};
        size_t _bufferCapacity{DefaultBufferSize};
        size_t _bufferPosition{0};

        auto FlushBuffer(bool flushStream) -> std::expected<void, Error>;

    public:
        explicit BinaryWriter(OutputStreamRef stream);

        BinaryWriter(OutputStreamRef stream, size_t bufferSize);

        BinaryWriter(BinaryWriter const&) = delete;

        BinaryWriter(BinaryWriter&&) = delete;

        ~BinaryWriter();

        BinaryWriter& operator=(BinaryWriter const&) = delete;

        BinaryWriter& operator=(BinaryWriter&&) = delete;

    public:
        auto Write(std::span<std::byte const> data) -> std::expected<size_t, Error>;

        auto Flush() -> std::expected<void, Error>;
    };
}

namespace Anemone
{
    inline auto operator<<(BinaryWriter& writer, float const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, double const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    auto operator<<(BinaryWriter& writer, long double const& value) -> BinaryWriter& = delete;

    inline auto operator<<(BinaryWriter& writer, std::byte const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, bool const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, signed char const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, unsigned char const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, signed short const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, unsigned short const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, signed int const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, unsigned int const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, signed long const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, unsigned long const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, signed long long const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline auto operator<<(BinaryWriter& writer, unsigned long long const& value) -> BinaryWriter&
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }
}
