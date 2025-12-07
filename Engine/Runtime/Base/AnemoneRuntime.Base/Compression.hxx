#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Diagnostics/Error.hxx"
#include "AnemoneRuntime.Base/FNV.hxx"
#include "AnemoneRuntime.Base/Intrusive.hxx"
#include "AnemoneRuntime.Base/FunctionRef.hxx"

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

        virtual std::expected<size_t, Error> Compress(
            std::span<std::byte> output,
            std::span<std::byte const> input) const = 0;

        virtual std::expected<size_t, Error> Decompress(
            std::span<std::byte> output,
            std::span<std::byte const> input) const = 0;

        virtual std::expected<size_t, Error> MemoryBound(size_t size) const = 0;

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
        ANEMONE_RUNTIME_BASE_API void Register(ICompressionFormat* format);

        ANEMONE_RUNTIME_BASE_API void Unregister(ICompressionFormat* format);

        ANEMONE_RUNTIME_BASE_API void Enumerate(FunctionRef<void(ICompressionFormat&)> callback);

        ANEMONE_RUNTIME_BASE_API ICompressionFormat* FindById(CompressionFormatId id) const;

        ANEMONE_RUNTIME_BASE_API ICompressionFormat* FindByName(std::string_view name) const;

        ANEMONE_RUNTIME_BASE_API static CompressionFormatRegistry& Get();
    };

    enum class CompressionMethod : uint32_t
    {
        Unknown,
        LZ4,
        LZ4HC,
        Default = LZ4,
    };

    ANEMONE_RUNTIME_BASE_API std::expected<size_t, Error> CompressionMemoryBound(
        CompressionMethod compressionMethod,
        size_t size);

    ANEMONE_RUNTIME_BASE_API std::expected<size_t, Error> CompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input);

    ANEMONE_RUNTIME_BASE_API std::expected<size_t, Error> DecompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input);
}
