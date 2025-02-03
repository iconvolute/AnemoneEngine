#include "AnemoneRuntime/Compression.hxx"

#include <lz4.h>
#include <lz4hc.h>

namespace Anemone
{
    std::expected<size_t, ErrorCode> CompressionMemoryBound(
        CompressionMethod compressionMethod,
        size_t size)
    {
        switch (compressionMethod)
        {
        case CompressionMethod::LZ4:
        case CompressionMethod::LZ4HC:
            {
                if (size > LZ4_MAX_INPUT_SIZE)
                {
                    // Input buffer is too large.
                    return std::unexpected(ErrorCode::InvalidArgument);
                }

                size_t const required = LZ4_compressBound(static_cast<int>(size));

                if (required == 0)
                {
                    // Input buffer is too large.
                    return std::unexpected(ErrorCode::InvalidBuffer);
                }

                return required;
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(ErrorCode::NotSupported);
    }

    std::expected<size_t, ErrorCode> CompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input)
    {
        switch (compressionMethod)
        {
        case CompressionMethod::LZ4:
        case CompressionMethod::LZ4HC:
            {
                if (input.size() > LZ4_MAX_INPUT_SIZE)
                {
                    // Input buffer is too large.
                    return std::unexpected(ErrorCode::InvalidArgument);
                }

                size_t const required = LZ4_compressBound(static_cast<int>(input.size()));

                if (required == 0)
                {
                    // Input buffer is too large.
                    return std::unexpected(ErrorCode::InvalidBuffer);
                }

                if (output.size() < required)
                {
                    // Output buffer is too small.
                    return std::unexpected(ErrorCode::InvalidBuffer);
                }

                int processed;

                if (compressionMethod == CompressionMethod::LZ4HC)
                {
                    processed = LZ4_compress_HC(
                        reinterpret_cast<const char*>(input.data()),
                        reinterpret_cast<char*>(output.data()),
                        static_cast<int>(input.size()),
                        static_cast<int>(output.size()),
                        LZ4HC_CLEVEL_MAX);
                }
                else
                {
                    processed = LZ4_compress_default(
                        reinterpret_cast<const char*>(input.data()),
                        reinterpret_cast<char*>(output.data()),
                        static_cast<int>(input.size()),
                        static_cast<int>(output.size()));
                }

                if (processed >= 0)
                {
                    return static_cast<size_t>(processed);
                }
                else
                {
                    return std::unexpected(ErrorCode::InvalidArgument);
                }
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(ErrorCode::NotSupported);
    }

    std::expected<size_t, ErrorCode> DecompressBlock(
        CompressionMethod compressionMethod,
        std::span<std::byte> output,
        std::span<std::byte const> input)
    {
        switch (compressionMethod)
        {
        case CompressionMethod::LZ4:
        case CompressionMethod::LZ4HC:
            {
                if ((input.size() > LZ4_MAX_INPUT_SIZE) or (output.size() > LZ4_MAX_INPUT_SIZE))
                {
                    // Input buffer is too large.
                    return std::unexpected(ErrorCode::InvalidArgument);
                }

                int processed = LZ4_decompress_safe(
                    reinterpret_cast<const char*>(input.data()),
                    reinterpret_cast<char*>(output.data()),
                    static_cast<int>(input.size()),
                    static_cast<int>(output.size()));

                if (processed < 0)
                {
                    return std::unexpected(ErrorCode::InvalidArgument);
                }

                return static_cast<size_t>(processed);
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(ErrorCode::NotSupported);
    }
}
