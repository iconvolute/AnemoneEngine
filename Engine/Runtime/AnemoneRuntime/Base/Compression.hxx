#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/ErrorCode.hxx"
#include "AnemoneRuntime/Hash/FNV.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"

#include <expected>
#include <cstdint>
#include <cstddef>
#include <span>

namespace Anemone
{
    enum class CompressionFlags : uint32_t
    {
        None = 0,
    };

    struct CompressionFormatId final
    {
        uint64_t Value;

        static constexpr CompressionFormatId FromString(std::string_view str)
        {
            return CompressionFormatId{FNV1A64::FromString(str)};
        }
    };

    class CompressionFormatRegistry;

    class ICompressionFormat : private IntrusiveListNode<ICompressionFormat, CompressionFormatRegistry>
    {
        friend struct IntrusiveList<ICompressionFormat, CompressionFormatRegistry>;

    public:
        virtual ~ICompressionFormat() = default;

        virtual std::expected<size_t, ErrorCode> Compress(
            std::span<std::byte> output,
            std::span<std::byte const> input) const = 0;

        virtual std::expected<size_t, ErrorCode> Decompress(
            std::span<std::byte> output,
            std::span<std::byte const> input) const = 0;

        virtual std::expected<size_t, ErrorCode> MemoryBound(size_t size) const = 0;

        virtual CompressionFormatId GetFormatId() const = 0;

        virtual std::string_view GetName() const = 0;
    };

    class CompressionFormatRegistry final
    {
    private:
        IntrusiveList<ICompressionFormat, CompressionFormatRegistry> m_formats{};

    public:
        CompressionFormatRegistry() = default;
        CompressionFormatRegistry(CompressionFormatRegistry const&) = delete;
        CompressionFormatRegistry(CompressionFormatRegistry&&) = delete;
        CompressionFormatRegistry& operator=(CompressionFormatRegistry const&) = delete;
        CompressionFormatRegistry& operator=(CompressionFormatRegistry&&) = delete;
        ~CompressionFormatRegistry() = default;

    public:
        RUNTIME_API void Register(ICompressionFormat* format);

        RUNTIME_API void Unregister(ICompressionFormat* format);

        RUNTIME_API void Enumerate(FunctionRef<void(ICompressionFormat&)> callback);

        RUNTIME_API ICompressionFormat* FindById(CompressionFormatId id) const;

        RUNTIME_API ICompressionFormat* FindByName(std::string_view name) const;

        RUNTIME_API static CompressionFormatRegistry& Get();
    };

    enum class CompressionMethod : uint32_t
    {
        Unknown,
        LZ4,
        LZ4HC,
        Default = LZ4,
    };

    RUNTIME_API std::expected<size_t, ErrorCode> CompressionMemoryBound(
        CompressionMethod compressionMethod,
        size_t size);

    RUNTIME_API std::expected<size_t, ErrorCode> CompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input);

    RUNTIME_API std::expected<size_t, ErrorCode> DecompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input);
}
