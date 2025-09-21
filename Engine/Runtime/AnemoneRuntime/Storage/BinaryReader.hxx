#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Storage/InputStream.hxx"

#include <memory>
#include <span>

namespace Anemone
{
    class RUNTIME_API BinaryReader final : public ReferenceCounted<BinaryReader>
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
        explicit BinaryReader(InputStreamRef stream);

        BinaryReader(InputStreamRef stream, size_t bufferSize);

        BinaryReader(BinaryReader const&) = delete;

        BinaryReader(BinaryReader&&) = delete;

        ~BinaryReader();

        BinaryReader& operator=(BinaryReader const&) = delete;

        BinaryReader& operator=(BinaryReader&&) = delete;

    public:
        bool EndOfStream();

        auto Read(std::span<std::byte> data) -> std::expected<size_t, Error>;
    };
}

namespace Anemone
{
    inline auto operator>>(BinaryReader& reader, float& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, double& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    auto operator>>(BinaryReader& reader, long double& value) -> std ::expected<size_t, Error> = delete;

    inline auto operator>>(BinaryReader& reader, std::byte value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, bool& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, signed char& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, unsigned char& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, signed short& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, unsigned short& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, signed int& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, unsigned int& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, signed long& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, unsigned long& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, signed long long& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }

    inline auto operator>>(BinaryReader& reader, unsigned long long& value) -> std ::expected<size_t, Error>
    {
        return reader.Read(std::as_writable_bytes(std::span{&value, 1}));
    }
}
