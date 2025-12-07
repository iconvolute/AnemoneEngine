#include "AnemoneRuntime.Graphics/PixelFormat.hxx"

namespace Anemone::Graphics
{
    struct PixelFormatDescriptor
    {
        PixelFormat Format;
        bool Compressed;
        uint16_t BlockSize;
        uint16_t PixelBits;
        uint16_t ColorBits;
        uint16_t AlphaBits;
        uint16_t DepthBits;
        uint16_t StencilBits;
        uint16_t ChannelsCount;
    };

    size_t GetBlockSize(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::BC1_UNorm:
        case PixelFormat::BC4_SNorm:
        case PixelFormat::BC4_UNorm:
            return 8;

        case PixelFormat::BC2_UNorm:
        case PixelFormat::BC3_UNorm:
        case PixelFormat::BC5_SNorm:
        case PixelFormat::BC5_UNorm:
        case PixelFormat::BC6H_SF16:
        case PixelFormat::BC6H_UF16:
        case PixelFormat::BC7_UNorm:
            return 16;

        default:
            break;
        }

        return 0;
    }

    size_t GetPixelBits(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::R8_UNorm:
        case PixelFormat::R8_SNorm:
        case PixelFormat::R8_UInt:
        case PixelFormat::R8_SInt:
        case PixelFormat::A8_UNorm:
            return 8;

        case PixelFormat::R16_Float:
        case PixelFormat::R16_UNorm:
        case PixelFormat::R16_SNorm:
        case PixelFormat::R16_UInt:
        case PixelFormat::R16_SInt:
        case PixelFormat::R8G8_UNorm:
        case PixelFormat::R8G8_SNorm:
        case PixelFormat::R8G8_UInt:
        case PixelFormat::R8G8_SInt:
        case PixelFormat::B5G6R5_UNorm:
        case PixelFormat::B5G5R5A1_UNorm:
        case PixelFormat::D16_UNorm:
            return 16;

        case PixelFormat::R32_Float:
        case PixelFormat::R32_UInt:
        case PixelFormat::R32_SInt:
        case PixelFormat::R16G16_Float:
        case PixelFormat::R16G16_UNorm:
        case PixelFormat::R16G16_SNorm:
        case PixelFormat::R16G16_UInt:
        case PixelFormat::R16G16_SInt:
        case PixelFormat::R11G11B10_Float:
        case PixelFormat::B8G8R8A8_UNorm:
        case PixelFormat::B8G8R8A8_UNorm_SRGB:
        case PixelFormat::B8G8R8X8_UNorm:
        case PixelFormat::R8G8B8A8_UNorm:
        case PixelFormat::R8G8B8A8_SNorm:
        case PixelFormat::R8G8B8A8_UInt:
        case PixelFormat::R8G8B8A8_SInt:
        case PixelFormat::R10G10B10A2_UNorm:
        case PixelFormat::R10G10B10A2_UInt:
        case PixelFormat::D24_UNorm_S8_UInt:
        case PixelFormat::D32_Float:
            return 32;

        case PixelFormat::R32G32_Float:
        case PixelFormat::R32G32_UInt:
        case PixelFormat::R32G32_SInt:
        case PixelFormat::R16G16B16A16_Float:
        case PixelFormat::R16G16B16A16_UNorm:
        case PixelFormat::R16G16B16A16_SNorm:
        case PixelFormat::R16G16B16A16_UInt:
        case PixelFormat::R16G16B16A16_SInt:
        case PixelFormat::D32_Float_S8X24_UInt:
            return 64;

        case PixelFormat::R32G32B32A32_Float:
        case PixelFormat::R32G32B32A32_UInt:
        case PixelFormat::R32G32B32A32_SInt:
            return 128;

        default:
            break;
        }

        return 0;
    }

    size_t GetColorBits(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::R8_SInt:
        case PixelFormat::R8_SNorm:
        case PixelFormat::R8_UInt:
        case PixelFormat::R8_UNorm:
            return 8;

        case PixelFormat::B5G6R5_UNorm:
        case PixelFormat::B5G5R5A1_UNorm:
        case PixelFormat::R16_Float:
        case PixelFormat::R16_SInt:
        case PixelFormat::R16_SNorm:
        case PixelFormat::R16_UInt:
        case PixelFormat::R16_UNorm:
        case PixelFormat::R8G8_SInt:
        case PixelFormat::R8G8_SNorm:
        case PixelFormat::R8G8_UInt:
        case PixelFormat::R8G8_UNorm:
            return 16;

        case PixelFormat::B8G8R8A8_UNorm:
        case PixelFormat::B8G8R8A8_UNorm_SRGB:
        case PixelFormat::B8G8R8X8_UNorm:
        case PixelFormat::R8G8B8A8_SInt:
        case PixelFormat::R8G8B8A8_SNorm:
        case PixelFormat::R8G8B8A8_UInt:
        case PixelFormat::R8G8B8A8_UNorm:
            return 24;

        case PixelFormat::R10G10B10A2_UInt:
        case PixelFormat::R10G10B10A2_UNorm:
            return 30;

        case PixelFormat::R11G11B10_Float:
        case PixelFormat::R16G16_Float:
        case PixelFormat::R16G16_SInt:
        case PixelFormat::R16G16_SNorm:
        case PixelFormat::R16G16_UInt:
        case PixelFormat::R16G16_UNorm:
        case PixelFormat::R32_Float:
        case PixelFormat::R32_SInt:
        case PixelFormat::R32_UInt:
            return 32;

        case PixelFormat::R16G16B16A16_Float:
        case PixelFormat::R16G16B16A16_SInt:
        case PixelFormat::R16G16B16A16_SNorm:
        case PixelFormat::R16G16B16A16_UInt:
        case PixelFormat::R16G16B16A16_UNorm:
            return 48;

        case PixelFormat::R32G32_Float:
        case PixelFormat::R32G32_SInt:
        case PixelFormat::R32G32_UInt:
            return 64;

        case PixelFormat::R32G32B32A32_Float:
        case PixelFormat::R32G32B32A32_SInt:
        case PixelFormat::R32G32B32A32_UInt:
            return 96;

        default:
            break;
        }

        return 0;
    }

    size_t GetAlphaBits(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::B5G5R5A1_UNorm:
            return 1;

        case PixelFormat::R10G10B10A2_UInt:
        case PixelFormat::R10G10B10A2_UNorm:
            return 2;

        case PixelFormat::A8_UNorm:
        case PixelFormat::R8G8B8A8_SInt:
        case PixelFormat::R8G8B8A8_SNorm:
        case PixelFormat::R8G8B8A8_UInt:
        case PixelFormat::B8G8R8A8_UNorm:
        case PixelFormat::R8G8B8A8_UNorm:
        case PixelFormat::B8G8R8A8_UNorm_SRGB:
            return 8;

        case PixelFormat::R16G16B16A16_Float:
        case PixelFormat::R16G16B16A16_SInt:
        case PixelFormat::R16G16B16A16_SNorm:
        case PixelFormat::R16G16B16A16_UInt:
        case PixelFormat::R16G16B16A16_UNorm:
            return 16;

        case PixelFormat::R32G32B32A32_Float:
        case PixelFormat::R32G32B32A32_SInt:
        case PixelFormat::R32G32B32A32_UInt:
            return 32;

        default:
            break;
        }

        return 0;
    }

    bool IsCompressed(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::BC1_UNorm:
        case PixelFormat::BC2_UNorm:
        case PixelFormat::BC3_UNorm:
        case PixelFormat::BC4_SNorm:
        case PixelFormat::BC4_UNorm:
        case PixelFormat::BC5_SNorm:
        case PixelFormat::BC5_UNorm:
        case PixelFormat::BC6H_SF16:
        case PixelFormat::BC6H_UF16:
        case PixelFormat::BC7_UNorm:
            return true;

        default:
            break;
        }

        return false;
    }

    size_t GetDepthBits(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::D16_UNorm:
            return 16;

        case PixelFormat::D24_UNorm_S8_UInt:
            return 24;

        case PixelFormat::D32_Float:
        case PixelFormat::D32_Float_S8X24_UInt:
            return 32;

        default:
            break;
        }

        return 0;
    }

    size_t GetStencilBits(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::D24_UNorm_S8_UInt:
        case PixelFormat::D32_Float_S8X24_UInt:
            return 8;

        default:
            break;
        }

        return 0;
    }

    size_t GetChannelsCount(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::R16_Float:
        case PixelFormat::R32_Float:
        case PixelFormat::R16_SInt:
        case PixelFormat::R32_SInt:
        case PixelFormat::R8_SInt:
        case PixelFormat::BC4_SNorm:
        case PixelFormat::R16_SNorm:
        case PixelFormat::R8_SNorm:
        case PixelFormat::R16_UInt:
        case PixelFormat::R32_UInt:
        case PixelFormat::R8_UInt:
        case PixelFormat::A8_UNorm:
        case PixelFormat::BC4_UNorm:
        case PixelFormat::R16_UNorm:
        case PixelFormat::R8_UNorm:
            return 1;

        case PixelFormat::R16G16_Float:
        case PixelFormat::R32G32_Float:
        case PixelFormat::R16G16_SInt:
        case PixelFormat::R32G32_SInt:
        case PixelFormat::R8G8_SInt:
        case PixelFormat::BC5_SNorm:
        case PixelFormat::R16G16_SNorm:
        case PixelFormat::R8G8_SNorm:
        case PixelFormat::R16G16_UInt:
        case PixelFormat::R32G32_UInt:
        case PixelFormat::R8G8_UInt:
        case PixelFormat::BC5_UNorm:
        case PixelFormat::R16G16_UNorm:
        case PixelFormat::R8G8_UNorm:
            return 2;

        case PixelFormat::R11G11B10_Float:
        case PixelFormat::B5G6R5_UNorm:
            return 3;

        case PixelFormat::BC6H_SF16:
        case PixelFormat::BC6H_UF16:
        case PixelFormat::R16G16B16A16_Float:
        case PixelFormat::R32G32B32A32_Float:
        case PixelFormat::R16G16B16A16_SInt:
        case PixelFormat::R32G32B32A32_SInt:
        case PixelFormat::R8G8B8A8_SInt:
        case PixelFormat::R16G16B16A16_SNorm:
        case PixelFormat::R8G8B8A8_SNorm:
        case PixelFormat::R10G10B10A2_UInt:
        case PixelFormat::R16G16B16A16_UInt:
        case PixelFormat::R32G32B32A32_UInt:
        case PixelFormat::R8G8B8A8_UInt:
        case PixelFormat::B8G8R8A8_UNorm:
        case PixelFormat::B8G8R8X8_UNorm:
        case PixelFormat::BC1_UNorm:
        case PixelFormat::BC2_UNorm:
        case PixelFormat::BC3_UNorm:
        case PixelFormat::BC7_UNorm:
        case PixelFormat::R10G10B10A2_UNorm:
        case PixelFormat::R16G16B16A16_UNorm:
        case PixelFormat::R8G8B8A8_UNorm:
        case PixelFormat::B8G8R8A8_UNorm_SRGB:
        case PixelFormat::B5G5R5A1_UNorm:
            return 4;

        default:
            break;
        }

        return 0;
    }
}
