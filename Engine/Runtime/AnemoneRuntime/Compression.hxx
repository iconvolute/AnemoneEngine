#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <cstdint>
#include <cstddef>
#include <span>

namespace Anemone
{
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
