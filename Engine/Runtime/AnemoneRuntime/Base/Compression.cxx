#include "AnemoneRuntime/Base/Compression.hxx"

#include <lz4.h>
#include <lz4hc.h>

namespace Anemone
{
    std::expected<size_t, Status> CompressionMemoryBound(
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
                    return std::unexpected(Status::InvalidArgument);
                }

                size_t const required = LZ4_compressBound(static_cast<int>(size));

                if (required == 0)
                {
                    // Input buffer is too large.
                    return std::unexpected(Status::InvalidBuffer);
                }

                return required;
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(Status::NotSupported);
    }

    std::expected<size_t, Status> CompressBlock(
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
                    return std::unexpected(Status::InvalidArgument);
                }

                size_t const required = LZ4_compressBound(static_cast<int>(input.size()));

                if (required == 0)
                {
                    // Input buffer is too large.
                    return std::unexpected(Status::InvalidBuffer);
                }

                if (output.size() < required)
                {
                    // Output buffer is too small.
                    return std::unexpected(Status::InvalidBuffer);
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
                    return std::unexpected(Status::InvalidArgument);
                }
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(Status::NotSupported);
    }

    std::expected<size_t, Status> DecompressBlock(
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
                    return std::unexpected(Status::InvalidArgument);
                }

                int processed = LZ4_decompress_safe(
                    reinterpret_cast<const char*>(input.data()),
                    reinterpret_cast<char*>(output.data()),
                    static_cast<int>(input.size()),
                    static_cast<int>(output.size()));

                if (processed < 0)
                {
                    return std::unexpected(Status::InvalidArgument);
                }

                return static_cast<size_t>(processed);
            }

        default:
        case CompressionMethod::Unknown:
            break;
        }

        return std::unexpected(Status::NotSupported);
    }
}
