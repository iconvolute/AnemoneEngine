#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstdint>
#include <cstddef>
#include <memory>

namespace Anemone::Graphics
{
    enum class PixelFormat
    {
        Unknown,

        R8_UNorm,
        R8_SNorm,
        R8_UInt,
        R8_SInt,
        A8_UNorm,
        R16_Float,
        R16_UNorm,
        R16_SNorm,
        R16_UInt,
        R16_SInt,
        R32_Float,
        R32_UInt,
        R32_SInt,
        R8G8_UNorm,
        R8G8_SNorm,
        R8G8_UInt,
        R8G8_SInt,
        R16G16_Float,
        R16G16_UNorm,
        R16G16_SNorm,
        R16G16_UInt,
        R16G16_SInt,
        R32G32_Float,
        R32G32_UInt,
        R32G32_SInt,
        B5G6R5_UNorm,
        B5G5R5A1_UNorm,
        R11G11B10_Float,
        B8G8R8A8_UNorm,
        B8G8R8A8_UNorm_SRGB,
        B8G8R8X8_UNorm,
        R8G8B8A8_UNorm,
        R8G8B8A8_SNorm,
        R8G8B8A8_UInt,
        R8G8B8A8_SInt,
        R10G10B10A2_UNorm,
        R10G10B10A2_UInt,
        R16G16B16A16_Float,
        R16G16B16A16_UNorm,
        R16G16B16A16_SNorm,
        R16G16B16A16_UInt,
        R16G16B16A16_SInt,
        R32G32B32A32_Float,
        R32G32B32A32_UInt,
        R32G32B32A32_SInt,

        D16_UNorm,
        D24_UNorm_S8_UInt,
        D32_Float,
        D32_Float_S8X24_UInt,

        BC1_UNorm,
        BC2_UNorm,
        BC3_UNorm,
        BC4_SNorm,
        BC4_UNorm,
        BC5_SNorm,
        BC5_UNorm,
        BC6H_SF16,
        BC6H_UF16,
        BC7_UNorm,
    };

    GRAPHICS_API size_t GetBlockSize(PixelFormat format);
    GRAPHICS_API size_t GetPixelBits(PixelFormat format);
    GRAPHICS_API size_t GetColorBits(PixelFormat format);
    GRAPHICS_API size_t GetAlphaBits(PixelFormat format);
    GRAPHICS_API bool IsCompressed(PixelFormat format);
    GRAPHICS_API size_t GetDepthBits(PixelFormat format);
    GRAPHICS_API size_t GetStencilBits(PixelFormat format);
    GRAPHICS_API size_t GetChannelsCount(PixelFormat format);

    struct ColorFloat
    {
        float R;
        float G;
        float B;
        float A;
    };

    struct ColorUInt
    {
        uint32_t R;
        uint32_t G;
        uint32_t B;
        uint32_t A;
    };

    template <typename T, size_t BitOffset, size_t BitCount, typename U>
    constexpr T ExtractBits(U value)
    {
        return static_cast<T>((value >> BitOffset) & ((U{1} << BitCount) - 1));
    }

    struct Pixel_10_10_10_2 final
    {
        uint32_t Inner;

        ColorUInt ToColorUInt() const
        {
            return ColorUInt{
                ExtractBits<uint32_t, 0, 10>(this->Inner),
                ExtractBits<uint32_t, 10, 10>(this->Inner),
                ExtractBits<uint32_t, 20, 10>(this->Inner),
                ExtractBits<uint32_t, 30, 2>(this->Inner)};
        }
    };
}
