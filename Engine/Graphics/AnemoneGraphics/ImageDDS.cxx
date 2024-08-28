#include "AnemoneGraphics/DxgiFormat.hxx"
#include "AnemoneGraphics/PixelFormat.hxx"
#include "AnemoneGraphics/Image.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

#include <vector>

namespace Anemone::Graphics::Private
{
    constexpr PixelFormat PixelFormatFromDxgi(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        default:
            break;

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return PixelFormat::R32G32B32A32_Float;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            return PixelFormat::R32G32B32A32_UInt;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return PixelFormat::R32G32B32A32_SInt;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return PixelFormat::R16G16B16A16_Float;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return PixelFormat::R16G16B16A16_UNorm;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            return PixelFormat::R16G16B16A16_UInt;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            return PixelFormat::R16G16B16A16_SNorm;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            return PixelFormat::R16G16B16A16_SInt;
        case DXGI_FORMAT_R32G32_FLOAT:
            return PixelFormat::R32G32_Float;
        case DXGI_FORMAT_R32G32_UINT:
            return PixelFormat::R32G32_UInt;
        case DXGI_FORMAT_R32G32_SINT:
            return PixelFormat::R32G32_SInt;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            return PixelFormat::D32_Float_S8X24_UInt;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            return PixelFormat::R10G10B10A2_UNorm;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            return PixelFormat::R10G10B10A2_UInt;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            return PixelFormat::R11G11B10_Float;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return PixelFormat::R8G8B8A8_UNorm;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            return PixelFormat::R8G8B8A8_UInt;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            return PixelFormat::R8G8B8A8_SNorm;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return PixelFormat::R8G8B8A8_SInt;
        case DXGI_FORMAT_R16G16_FLOAT:
            return PixelFormat::R16G16_Float;
        case DXGI_FORMAT_R16G16_UNORM:
            return PixelFormat::R16G16_UNorm;
        case DXGI_FORMAT_R16G16_UINT:
            return PixelFormat::R16G16_UInt;
        case DXGI_FORMAT_R16G16_SNORM:
            return PixelFormat::R16G16_SNorm;
        case DXGI_FORMAT_R16G16_SINT:
            return PixelFormat::R16G16_SInt;
        case DXGI_FORMAT_D32_FLOAT:
            return PixelFormat::D32_Float;
        case DXGI_FORMAT_R32_FLOAT:
            return PixelFormat::R32_Float;
        case DXGI_FORMAT_R32_UINT:
            return PixelFormat::R32_UInt;
        case DXGI_FORMAT_R32_SINT:
            return PixelFormat::R32_SInt;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            return PixelFormat::D24_UNorm_S8_UInt;
        case DXGI_FORMAT_R8G8_UNORM:
            return PixelFormat::R8G8_UNorm;
        case DXGI_FORMAT_R8G8_UINT:
            return PixelFormat::R8G8_UInt;
        case DXGI_FORMAT_R8G8_SNORM:
            return PixelFormat::R8G8_SNorm;
        case DXGI_FORMAT_R8G8_SINT:
            return PixelFormat::R8G8_SInt;
        case DXGI_FORMAT_R16_FLOAT:
            return PixelFormat::R16_Float;
        case DXGI_FORMAT_D16_UNORM:
            return PixelFormat::D16_UNorm;
        case DXGI_FORMAT_R16_UNORM:
            return PixelFormat::R16_UNorm;
        case DXGI_FORMAT_R16_UINT:
            return PixelFormat::R16_UInt;
        case DXGI_FORMAT_R16_SNORM:
            return PixelFormat::R16_SNorm;
        case DXGI_FORMAT_R16_SINT:
            return PixelFormat::R16_SInt;
        case DXGI_FORMAT_R8_UNORM:
            return PixelFormat::R8_UNorm;
        case DXGI_FORMAT_R8_UINT:
            return PixelFormat::R8_UInt;
        case DXGI_FORMAT_R8_SNORM:
            return PixelFormat::R8_SNorm;
        case DXGI_FORMAT_R8_SINT:
            return PixelFormat::R8_SInt;
        case DXGI_FORMAT_A8_UNORM:
            return PixelFormat::A8_UNorm;
        case DXGI_FORMAT_BC1_UNORM:
            return PixelFormat::BC1_UNorm;
        case DXGI_FORMAT_BC2_UNORM:
            return PixelFormat::BC2_UNorm;
        case DXGI_FORMAT_BC3_UNORM:
            return PixelFormat::BC3_UNorm;
        case DXGI_FORMAT_BC4_UNORM:
            return PixelFormat::BC4_UNorm;
        case DXGI_FORMAT_BC4_SNORM:
            return PixelFormat::BC4_SNorm;
        case DXGI_FORMAT_BC5_UNORM:
            return PixelFormat::BC5_UNorm;
        case DXGI_FORMAT_BC5_SNORM:
            return PixelFormat::BC5_SNorm;
        case DXGI_FORMAT_B5G6R5_UNORM:
            return PixelFormat::B5G6R5_UNorm;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            return PixelFormat::B5G5R5A1_UNorm;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return PixelFormat::B8G8R8A8_UNorm;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            return PixelFormat::B8G8R8X8_UNorm;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return PixelFormat::B8G8R8A8_UNorm_SRGB;
        case DXGI_FORMAT_BC6H_UF16:
            return PixelFormat::BC6H_UF16;
        case DXGI_FORMAT_BC6H_SF16:
            return PixelFormat::BC6H_SF16;
        case DXGI_FORMAT_BC7_UNORM:
            return PixelFormat::BC7_UNorm;
        }

        return PixelFormat::Unknown;
    }

    constexpr DXGI_FORMAT PixelFormatToDxgi(PixelFormat format)
    {
        switch (format)
        {
        case PixelFormat::Unknown:
            break;

        case PixelFormat::R8_UNorm:
            return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::R8_SNorm:
            return DXGI_FORMAT_R8_SNORM;
        case PixelFormat::R8_UInt:
            return DXGI_FORMAT_R8_UINT;
        case PixelFormat::R8_SInt:
            return DXGI_FORMAT_R8_SINT;
        case PixelFormat::A8_UNorm:
            return DXGI_FORMAT_A8_UNORM;
        case PixelFormat::R16_Float:
            return DXGI_FORMAT_R16_FLOAT;
        case PixelFormat::R16_UNorm:
            return DXGI_FORMAT_R16_UNORM;
        case PixelFormat::R16_SNorm:
            return DXGI_FORMAT_R16_SNORM;
        case PixelFormat::R16_UInt:
            return DXGI_FORMAT_R16_UINT;
        case PixelFormat::R16_SInt:
            return DXGI_FORMAT_R16_SINT;
        case PixelFormat::R32_Float:
            return DXGI_FORMAT_R32_FLOAT;
        case PixelFormat::R32_UInt:
            return DXGI_FORMAT_R32_UINT;
        case PixelFormat::R32_SInt:
            return DXGI_FORMAT_R32_SINT;
        case PixelFormat::R8G8_UNorm:
            return DXGI_FORMAT_R8G8_UNORM;
        case PixelFormat::R8G8_SNorm:
            return DXGI_FORMAT_R8G8_SNORM;
        case PixelFormat::R8G8_UInt:
            return DXGI_FORMAT_R8G8_UINT;
        case PixelFormat::R8G8_SInt:
            return DXGI_FORMAT_R8G8_SINT;
        case PixelFormat::R16G16_Float:
            return DXGI_FORMAT_R16G16_FLOAT;
        case PixelFormat::R16G16_UNorm:
            return DXGI_FORMAT_R16G16_UNORM;
        case PixelFormat::R16G16_SNorm:
            return DXGI_FORMAT_R16G16_SNORM;
        case PixelFormat::R16G16_UInt:
            return DXGI_FORMAT_R16G16_UINT;
        case PixelFormat::R16G16_SInt:
            return DXGI_FORMAT_R16G16_SINT;
        case PixelFormat::R32G32_Float:
            return DXGI_FORMAT_R32G32_FLOAT;
        case PixelFormat::R32G32_UInt:
            return DXGI_FORMAT_R32G32_UINT;
        case PixelFormat::R32G32_SInt:
            return DXGI_FORMAT_R32G32_SINT;
        case PixelFormat::B5G6R5_UNorm:
            return DXGI_FORMAT_B5G6R5_UNORM;
        case PixelFormat::B5G5R5A1_UNorm:
            return DXGI_FORMAT_B5G5R5A1_UNORM;
        case PixelFormat::R11G11B10_Float:
            return DXGI_FORMAT_R11G11B10_FLOAT;
        case PixelFormat::B8G8R8A8_UNorm:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::B8G8R8A8_UNorm_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case PixelFormat::B8G8R8X8_UNorm:
            return DXGI_FORMAT_B8G8R8X8_UNORM;
        case PixelFormat::R8G8B8A8_UNorm:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::R8G8B8A8_SNorm:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case PixelFormat::R8G8B8A8_UInt:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case PixelFormat::R8G8B8A8_SInt:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case PixelFormat::R10G10B10A2_UNorm:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case PixelFormat::R10G10B10A2_UInt:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case PixelFormat::R16G16B16A16_Float:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::R16G16B16A16_UNorm:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case PixelFormat::R16G16B16A16_SNorm:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case PixelFormat::R16G16B16A16_UInt:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case PixelFormat::R16G16B16A16_SInt:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case PixelFormat::R32G32B32A32_Float:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::R32G32B32A32_UInt:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case PixelFormat::R32G32B32A32_SInt:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case PixelFormat::D16_UNorm:
            return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::D24_UNorm_S8_UInt:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::D32_Float:
            return DXGI_FORMAT_D32_FLOAT;
        case PixelFormat::D32_Float_S8X24_UInt:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case PixelFormat::BC1_UNorm:
            return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::BC2_UNorm:
            return DXGI_FORMAT_BC2_UNORM;
        case PixelFormat::BC3_UNorm:
            return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::BC4_SNorm:
            return DXGI_FORMAT_BC4_SNORM;
        case PixelFormat::BC4_UNorm:
            return DXGI_FORMAT_BC4_UNORM;
        case PixelFormat::BC5_SNorm:
            return DXGI_FORMAT_BC5_SNORM;
        case PixelFormat::BC5_UNorm:
            return DXGI_FORMAT_BC5_UNORM;
        case PixelFormat::BC6H_SF16:
            return DXGI_FORMAT_BC6H_SF16;
        case PixelFormat::BC6H_UF16:
            return DXGI_FORMAT_BC6H_UF16;
        case PixelFormat::BC7_UNorm:
            return DXGI_FORMAT_BC7_UNORM;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    constexpr uint32_t DDS_PIXELFORMAT_FOURCC = 0x00000004u;
    constexpr uint32_t DDS_PIXELFORMAT_RGB = 0x00000040u;
    constexpr uint32_t DDS_PIXELFORMAT_LUMINANCE = 0x00020000u;
    constexpr uint32_t DDS_PIXELFORMAT_ALPHA = 0x00000002u;
    constexpr uint32_t DDS_PIXELFORMAT_BUMPDUDV = 0x00080000u;

    consteval uint32_t MakeFourCC(
        uint32_t c0,
        uint32_t c1,
        uint32_t c2,
        uint32_t c3)
    {
        return (c0) | (c1 << 8u) | (c2 << 16u) | (c3 << 24u);
    }

    constexpr uint32_t D3DFMT_A16B16G16R16 = 36;
    constexpr uint32_t D3DFMT_Q16W16V16U16 = 110;
    constexpr uint32_t D3DFMT_R16F = 111;
    constexpr uint32_t D3DFMT_G16R16F = 112;
    constexpr uint32_t D3DFMT_A16B16G16R16F = 113;
    constexpr uint32_t D3DFMT_R32F = 114;
    constexpr uint32_t D3DFMT_G32R32F = 115;
    constexpr uint32_t D3DFMT_A32B32G32R32F = 116;

    struct DDS_PIXELFORMAT final
    {
        uint32_t Size;
        uint32_t Flags;
        uint32_t FourCC;
        uint32_t RGBBitCount;
        uint32_t RBitMask;
        uint32_t GBitMask;
        uint32_t BBitMask;
        uint32_t ABitMask;

        [[nodiscard]] constexpr bool IsBitMask(uint32_t r, uint32_t g, uint32_t b, uint32_t a) const
        {
            return (RBitMask == r) and (GBitMask == g) and (BBitMask == b) and (ABitMask == a);
        }
    };

    constexpr DXGI_FORMAT GetFormat(DDS_PIXELFORMAT const& format)
    {
        if (format.Flags & DDS_PIXELFORMAT_RGB)
        {
            switch (format.RGBBitCount)
            {
            case 32:
                if (format.IsBitMask(0xFFu << 0u, 0xFFu << 8u, 0xFFu << 16u, 0xFFu << 24u))
                {
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }

                if (format.IsBitMask(0xFFu << 16u, 0xFFu << 8u, 0xFFu << 0u, 0xFFu << 24u))
                {
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                }

                if (format.IsBitMask(0xFFu << 16u, 0xFFu << 8u, 0xFFu << 0u, 0))
                {
                    return DXGI_FORMAT_B8G8R8X8_UNORM;
                }

                if (format.IsBitMask(0x3FFu << 20u, 0x3FFu << 10u, 0x3FF << 0u, 0x3u << 30u))
                {
                    return DXGI_FORMAT_R10G10B10A2_UNORM;
                }

                if (format.IsBitMask(0xFFFFu << 0u, 0xFFFFu << 16u, 0, 0))
                {
                    return DXGI_FORMAT_R16G16_UNORM;
                }

                if (format.IsBitMask(0xFFFFFFFFu, 0, 0, 0))
                {
                    return DXGI_FORMAT_R32_FLOAT;
                }
                break;

            case 16:
                if (format.IsBitMask(0x1Fu << 10u, 0x1Fu << 5u, 0x1Fu << 0u, 0x1u << 15u))
                {
                    return DXGI_FORMAT_B5G5R5A1_UNORM;
                }

                if (format.IsBitMask(0x1Fu << 11u, 0x3Fu << 5u, 0x1Fu << 0u, 0))
                {
                    return DXGI_FORMAT_B5G6R5_UNORM;
                }

                if (format.IsBitMask(0xFu << 8u, 0xFu << 4u, 0xFu << 0u, 0xFu << 12u))
                {
                    return DXGI_FORMAT_B4G4R4A4_UNORM;
                }

                if (format.IsBitMask(0xFFu << 0u, 0, 0, 0xFFu << 8u))
                {
                    return DXGI_FORMAT_R8G8_UNORM;
                }

                if (format.IsBitMask(0xFFFFu << 0u, 0, 0, 0))
                {
                    return DXGI_FORMAT_R16_UNORM;
                }
                break;

            case 8:
                if (format.IsBitMask(0xFFu << 0u, 0, 0, 0))
                {
                    return DXGI_FORMAT_R8_UNORM;
                }
                break;

            default:
                break;
            }
        }
        else if (format.Flags & DDS_PIXELFORMAT_LUMINANCE)
        {
            switch (format.RGBBitCount)
            {
            case 16:
                if (format.IsBitMask(0xFFFFu << 0u, 0, 0, 0))
                {
                    return DXGI_FORMAT_R16_UNORM;
                }

                if (format.IsBitMask(0xFFu << 0u, 0, 0, 0xFFu << 8u))
                {
                    return DXGI_FORMAT_R8G8_UNORM;
                }
                break;

            case 8:
                if (format.IsBitMask(0xFFu << 0u, 0, 0, 0))
                {
                    return DXGI_FORMAT_R8_UNORM;
                }

                if (format.IsBitMask(0xFFu << 0u, 0, 0, 0xFFu << 8u))
                {
                    return DXGI_FORMAT_R8G8_UNORM;
                }
                break;

            default:
                break;
            }
        }
        else if (format.Flags & DDS_PIXELFORMAT_ALPHA)
        {
            if (format.RGBBitCount == 8)
            {
                return DXGI_FORMAT_A8_UNORM;
            }
        }
        else if (format.Flags & DDS_PIXELFORMAT_BUMPDUDV)
        {
            switch (format.RGBBitCount)
            {
            case 32:
                if (format.IsBitMask(0xFFu << 0u, 0xFFu << 8u, 0xFFu << 16u, 0xFFu << 24u))
                {
                    return DXGI_FORMAT_R8G8B8A8_SNORM;
                }

                if (format.IsBitMask(0xFFFFu << 0u, 0xFFFFu << 16u, 0, 0))
                {
                    return DXGI_FORMAT_R16G16_SNORM;
                }

                break;

            case 16:
                if (format.IsBitMask(0xFFu << 0u, 0xFFu << 8u, 0, 0))
                {
                    return DXGI_FORMAT_R8G8B8A8_SNORM;
                }
                break;

            default:
                break;
            }
        }
        else if (format.Flags & DDS_PIXELFORMAT_FOURCC)
        {
            switch (format.FourCC)
            {
            case MakeFourCC('D', 'X', 'T', '1'):
                return DXGI_FORMAT_BC1_UNORM;

            case MakeFourCC('D', 'X', 'T', '3'):
                return DXGI_FORMAT_BC2_UNORM;

            case MakeFourCC('D', 'X', 'T', '5'):
                return DXGI_FORMAT_BC3_UNORM;

            case MakeFourCC('D', 'X', 'T', '2'):
                return DXGI_FORMAT_BC2_UNORM;

            case MakeFourCC('D', 'X', 'T', '4'):
                return DXGI_FORMAT_BC3_UNORM;

            case MakeFourCC('A', 'T', 'I', '1'):
            case MakeFourCC('B', 'C', '4', 'U'):
                return DXGI_FORMAT_BC4_UNORM;

            case MakeFourCC('B', 'C', '4', 'S'):
                return DXGI_FORMAT_BC4_SNORM;

            case MakeFourCC('A', 'T', 'I', '2'):
            case MakeFourCC('B', 'C', '5', 'U'):
                return DXGI_FORMAT_BC5_UNORM;

            case MakeFourCC('B', 'C', '5', 'S'):
                return DXGI_FORMAT_BC5_SNORM;

            case MakeFourCC('R', 'G', 'B', 'G'):
                return DXGI_FORMAT_R8G8_B8G8_UNORM;

            case MakeFourCC('G', 'R', 'G', 'B'):
                return DXGI_FORMAT_G8R8_G8B8_UNORM;

            case MakeFourCC('Y', 'U', 'Y', '2'):
                return DXGI_FORMAT_YUY2;

            case D3DFMT_A16B16G16R16:
                return DXGI_FORMAT_R16G16B16A16_UNORM;

            case D3DFMT_Q16W16V16U16:
                return DXGI_FORMAT_R16G16B16A16_SNORM;

            case D3DFMT_R16F:
                return DXGI_FORMAT_R16_FLOAT;

            case D3DFMT_G16R16F:
                return DXGI_FORMAT_R16G16_FLOAT;

            case D3DFMT_A16B16G16R16F:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;

            case D3DFMT_R32F:
                return DXGI_FORMAT_R32_FLOAT;

            case D3DFMT_G32R32F:
                return DXGI_FORMAT_R32G32_FLOAT;

            case D3DFMT_A32B32G32R32F:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;

            default:
                break;
            }
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    constexpr DXGI_FORMAT MakeSRGB(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case DXGI_FORMAT_BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM_SRGB;

        case DXGI_FORMAT_BC2_UNORM:
            return DXGI_FORMAT_BC2_UNORM_SRGB;

        case DXGI_FORMAT_BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM_SRGB;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
            return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

        case DXGI_FORMAT_BC7_UNORM:
            return DXGI_FORMAT_BC7_UNORM_SRGB;

        default:
            return format;
        }
    }

    constexpr DXGI_FORMAT MakeLinear(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case DXGI_FORMAT_BC1_UNORM_SRGB:
            return DXGI_FORMAT_BC1_UNORM;

        case DXGI_FORMAT_BC2_UNORM_SRGB:
            return DXGI_FORMAT_BC2_UNORM;

        case DXGI_FORMAT_BC3_UNORM_SRGB:
            return DXGI_FORMAT_BC3_UNORM;

        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM;

        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8X8_UNORM;

        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return DXGI_FORMAT_BC7_UNORM;

        default:
            return format;
        }
    }

    constexpr bool IsDepthStencil(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_D16_UNORM:
            return true;

        default:
            return false;
        }
    }

    constexpr bool IsCompressed(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return true;

        default:
            return false;
        }
    }

    constexpr DXGI_FORMAT EnsureNotTypeless(DXGI_FORMAT format)
    {
        // Assumes UNORM or FLOAT; doesn't use UINT or SINT
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R32G32_TYPELESS:
            return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_R16G16_TYPELESS:
            return DXGI_FORMAT_R16G16_UNORM;
        case DXGI_FORMAT_R32_TYPELESS:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R8G8_TYPELESS:
            return DXGI_FORMAT_R8G8_UNORM;
        case DXGI_FORMAT_R16_TYPELESS:
            return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_R8_TYPELESS:
            return DXGI_FORMAT_R8_UNORM;
        case DXGI_FORMAT_BC1_TYPELESS:
            return DXGI_FORMAT_BC1_UNORM;
        case DXGI_FORMAT_BC2_TYPELESS:
            return DXGI_FORMAT_BC2_UNORM;
        case DXGI_FORMAT_BC3_TYPELESS:
            return DXGI_FORMAT_BC3_UNORM;
        case DXGI_FORMAT_BC4_TYPELESS:
            return DXGI_FORMAT_BC4_UNORM;
        case DXGI_FORMAT_BC5_TYPELESS:
            return DXGI_FORMAT_BC5_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            return DXGI_FORMAT_B8G8R8X8_UNORM;
        case DXGI_FORMAT_BC7_TYPELESS:
            return DXGI_FORMAT_BC7_UNORM;
        default:
            return format;
        }
    }

    constexpr size_t GetBitsPerPixel(DXGI_FORMAT format)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 128;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 96;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_Y416:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            return 64;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
        case DXGI_FORMAT_Y410:
        case DXGI_FORMAT_YUY2:
            return 32;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
        case DXGI_FORMAT_V408:
            return 24;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
        case DXGI_FORMAT_P208:
        case DXGI_FORMAT_V208:
            return 16;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
        case DXGI_FORMAT_NV11:
            return 12;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
            return 8;

        case DXGI_FORMAT_R1_UNORM:
            return 1;

        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            return 4;

        default:
            break;
        }

        return 0;
    }


    constexpr uint32_t DDS_HEADER_SIGNATURE = 0x20534444;

    constexpr uint32_t DDS_HEADER_CAPS = 0x1;
    constexpr uint32_t DDS_HEADER_HEIGHT = 0x2;
    constexpr uint32_t DDS_HEADER_WIDTH = 0x4;
    constexpr uint32_t DDS_HEADER_PITCH = 0x8;
    constexpr uint32_t DDS_HEADER_PIXELFORMAT = 0x1000;
    constexpr uint32_t DDS_HEADER_MIPMAP = 0x20000;
    constexpr uint32_t DDS_HEADER_LINEARSIZE = 0x80000;
    constexpr uint32_t DDS_HEADER_VOLUME = 0x800000;

    constexpr uint32_t DDS_HEADER_TEXTURE = DDS_HEADER_CAPS | DDS_HEADER_HEIGHT | DDS_HEADER_WIDTH | DDS_HEADER_PIXELFORMAT;

    constexpr uint32_t DDS_SURFACE_TEXTURE = 0x1000;
    constexpr uint32_t DDS_SURFACE_MIPMAP = 0x400008;
    constexpr uint32_t DDS_SURFACE_CUBEMAP = 0x8;

    constexpr uint32_t DDS_CUBEMAP = 0x200;
    constexpr uint32_t DDS_CUBEMAP_POSITIVEX = 0x600;
    constexpr uint32_t DDS_CUBEMAP_NEGATIVEX = 0xa00;
    constexpr uint32_t DDS_CUBEMAP_POSITIVEY = 0x1200;
    constexpr uint32_t DDS_CUBEMAP_NEGATIVEY = 0x2200;
    constexpr uint32_t DDS_CUBEMAP_POSITIVEZ = 0x4200;
    constexpr uint32_t DDS_CUBEMAP_NEGATIVEZ = 0x8200;

    constexpr const uint32_t DDS_CUBEMAP_ALLFACES = DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ;

    constexpr const uint32_t DDS_CUBEMAP_VOLUME = 0x200000;

    struct DDS_HEADER final
    {
        uint32_t Size;
        uint32_t Flags;
        uint32_t Height;
        uint32_t Width;
        uint32_t PitchOrLinearSize;
        uint32_t Depth;
        uint32_t MipMapCount;
        uint32_t Reserved1[11];
        DDS_PIXELFORMAT PixelFormat;
        uint32_t Surface;
        uint32_t Cubemap;
        uint32_t Caps3;
        uint32_t Caps4;
        uint32_t Reserved2;
    };
    static_assert(sizeof(DDS_HEADER) == 124);

    constexpr uint32_t DDS_RESOURCE_DIMENSION_TEXTURE_1D = 2;
    constexpr uint32_t DDS_RESOURCE_DIMENSION_TEXTURE_2D = 3;
    constexpr uint32_t DDS_RESOURCE_DIMENSION_TEXTURE_3D = 4;

    constexpr uint32_t DDS_RESOURCE_MISC_FLAG_TEXTURE_CUBE = 0x4;

    constexpr uint32_t DDS_ALPHA_MODE_UNKNOWN = 0;
    constexpr uint32_t DDS_ALPHA_MODE_STRAIGHT = 1;
    constexpr uint32_t DDS_ALPHA_MODE_PREMULTIPLIED = 2;
    constexpr uint32_t DDS_ALPHA_MODE_OPAQUE = 3;
    constexpr uint32_t DDS_ALPHA_MODE_CUSTOM = 4;

    constexpr uint32_t DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 7;

    struct DDS_HEADER_DXT10 final
    {
        DXGI_FORMAT Format;
        uint32_t ResourceDimension;
        uint32_t MiscFlags;
        uint32_t ArraySize;
        uint32_t MiscFlags2;
    };
    static_assert(sizeof(DDS_HEADER_DXT10) == 20);

    constexpr uint32_t DDS_TEXTURE_1D_U_DIMENSION = 16384;
    constexpr uint32_t DDS_TEXTURE_1D_ARRAY_AXIS_DIMENSION = 2048;
    constexpr uint32_t DDS_TEXTURE_2D_UV_DIMENSION = 16384;
    constexpr uint32_t DDS_TEXTURE_2D_ARRAY_AXIS_DIMENSION = 2048;
    constexpr uint32_t DDS_TEXTURE_3D_UVW_DIMENSION = 2048;
    constexpr uint32_t DDS_TEXTURE_CUBE_DIMENSION = 16384;

    constexpr size_t DDS_MAX_SUBRESOURCES = 1u << 16u;
    constexpr size_t DDS_MAX_MIP_LEVELS = 1u << 11u;

    constexpr bool GetSurfaceInfo(
        size_t width,
        size_t height,
        DXGI_FORMAT format,
        size_t* outBytesCount,
        size_t* outRowBytes,
        size_t* outRowsCount)
    {
        uint64_t bytesCount;
        uint64_t rowBytes;
        uint64_t rowsCount;

        bool bc = false;
        bool packed = false;
        bool planar = false;
        size_t bpe = 0;

        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            bc = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
        case DXGI_FORMAT_BC6H_TYPELESS:
        case DXGI_FORMAT_BC6H_UF16:
        case DXGI_FORMAT_BC6H_SF16:
        case DXGI_FORMAT_BC7_TYPELESS:
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            bc = true;
            bpe = 16;
            break;

        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_YUY2:
            packed = true;
            bpe = 4;
            break;

        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
            packed = true;
            bpe = 8;
            break;

        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_420_OPAQUE:
            if ((height % 2) != 0)
            {
                // Requires a height alignment of 2.
                return false;
            }

            planar = true;
            bpe = 2;
            break;

        case DXGI_FORMAT_P208:
            planar = true;
            bpe = 2;
            break;

        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            if ((height % 2) != 0)
            {
                // Requires a height alignment of 2.
                return false;
            }
            planar = true;
            bpe = 4;
            break;

        default:
            break;
        }

        if (bc)
        {
            uint64_t numBlocksWide = 0;
            if (width > 0)
            {
                numBlocksWide = std::max<uint64_t>(1u, (static_cast<uint64_t>(width) + 3u) / 4u);
            }
            uint64_t numBlocksHigh = 0;
            if (height > 0)
            {
                numBlocksHigh = std::max<uint64_t>(1u, (static_cast<uint64_t>(height) + 3u) / 4u);
            }
            rowBytes = numBlocksWide * bpe;
            rowsCount = numBlocksHigh;
            bytesCount = rowBytes * numBlocksHigh;
        }
        else if (packed)
        {
            rowBytes = ((static_cast<uint64_t>(width) + 1u) >> 1) * bpe;
            rowsCount = static_cast<uint64_t>(height);
            bytesCount = rowBytes * height;
        }
        else if (format == DXGI_FORMAT_NV11)
        {
            rowBytes = ((static_cast<uint64_t>(width) + 3u) >> 2) * 4u;
            rowsCount = static_cast<uint64_t>(height) * 2u;
            bytesCount = rowBytes * rowsCount;
        }
        else if (planar)
        {
            rowBytes = ((static_cast<uint64_t>(width) + 1u) >> 1) * bpe;
            rowsCount = (rowBytes * static_cast<uint64_t>(height)) + ((rowBytes * static_cast<uint64_t>(height) + 1u) >> 1);
            bytesCount = height + ((static_cast<uint64_t>(height) + 1u) >> 1);
        }
        else
        {
            const size_t bpp = GetBitsPerPixel(format);
            if (!bpp)
                return false;

            rowBytes = (static_cast<uint64_t>(width) * bpp + 7u) / 8u; // round up to nearest byte
            rowsCount = static_cast<uint64_t>(height);
            bytesCount = rowBytes * height;
        }

        if (outBytesCount)
        {
            *outBytesCount = static_cast<size_t>(bytesCount);
        }

        if (outRowBytes)
        {
            *outRowBytes = static_cast<size_t>(rowBytes);
        }

        if (outRowsCount)
        {
            *outRowsCount = static_cast<size_t>(rowsCount);
        }

        return true;
    }

    constexpr void AdjustPlanarResource(DXGI_FORMAT format, size_t height, size_t slice, ImageSurface& surface)
    {
        switch (format) // NOLINT(clang-diagnostic-switch-enum)
        {
        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
            if (not slice)
            {
                surface.SlicePitch = surface.LinePitch * static_cast<size_t>(height);
            }
            else
            {
                surface.Buffer = surface.Buffer + surface.LinePitch * height;
                surface.SlicePitch = surface.LinePitch * ((height + 1) >> 1);
            }
            break;

        case DXGI_FORMAT_NV11:
            if (not slice)
            {
                surface.SlicePitch = surface.LinePitch * static_cast<size_t>(height);
            }
            else
            {
                surface.Buffer = surface.Buffer + surface.LinePitch * height;
                surface.LinePitch = surface.LinePitch >> 1;
                surface.SlicePitch = surface.LinePitch * height;
            }
            break;

        default:
            break;
        }
    }

    constexpr uint32_t CountMips(uint32_t width, uint32_t height)
    {
        if ((width == 0) or (height == 0))
        {
            return 0;
        }

        uint32_t count = 1;

        while ((width > 1) or (height > 1))
        {
            width >>= 1;
            height >>= 1;
            count++;
        }
        return count;
    }

    uint32_t ConvertAlphaMode_DXT10(ImageAlphaMode value)
    {
        switch (value)
        {
        case ImageAlphaMode::Straight:
            return DDS_ALPHA_MODE_STRAIGHT;
        case ImageAlphaMode::Premultiplied:
            return DDS_ALPHA_MODE_PREMULTIPLIED;
        case ImageAlphaMode::Opaque:
            return DDS_ALPHA_MODE_OPAQUE;
        case ImageAlphaMode::Custom:
            return DDS_ALPHA_MODE_CUSTOM;
        case ImageAlphaMode::Unknown:
            break;
        }

        return DDS_ALPHA_MODE_UNKNOWN;
    }

    constexpr ImageAlphaMode GetAlphaMode(
        DDS_HEADER const* header,
        DDS_HEADER_DXT10 const* header10)
    {
        if (header->Flags & DDS_PIXELFORMAT_FOURCC)
        {
            if (header->PixelFormat.FourCC == MakeFourCC('D', 'X', '1', '0'))
            {
                switch (header10->MiscFlags2 & DDS_MISC_FLAGS2_ALPHA_MODE_MASK)
                {
                case DDS_ALPHA_MODE_STRAIGHT:
                    return ImageAlphaMode::Straight;

                case DDS_ALPHA_MODE_PREMULTIPLIED:
                    return ImageAlphaMode::Premultiplied;

                case DDS_ALPHA_MODE_OPAQUE:
                    return ImageAlphaMode::Opaque;

                case DDS_ALPHA_MODE_CUSTOM:
                    return ImageAlphaMode::Custom;

                case DDS_ALPHA_MODE_UNKNOWN:
                default:
                    break;
                }
            }
            else if ((header->PixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '2')) or
                (header->PixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '4')))
            {
                return ImageAlphaMode::Premultiplied;
            }
        }

        return ImageAlphaMode::Unknown;
    }

    std::expected<void, ErrorCode> ParseFileHeader(
        std::span<std::byte> data,
        DDS_HEADER const*& header,
        DDS_HEADER_DXT10 const*& header10,
        std::span<std::byte>& resources)
    {
        if (data.size() > std::numeric_limits<uint32_t>::max())
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (data.size() < (sizeof(uint32_t) + sizeof(DDS_HEADER)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        uint32_t const signature = *reinterpret_cast<uint32_t const*>(data.data());

        if (signature != DDS_HEADER_SIGNATURE)
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        header = reinterpret_cast<DDS_HEADER const*>(data.data() + sizeof(uint32_t));

        if ((header->Size != sizeof(DDS_HEADER)) or (header->PixelFormat.Size != sizeof(DDS_PIXELFORMAT)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        header10 = nullptr;

        if ((header->PixelFormat.Flags & DDS_PIXELFORMAT_FOURCC) and (header->PixelFormat.FourCC == MakeFourCC('D', 'X', '1', '0')))
        {
            if (data.size() < (sizeof(uint32_t) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10)))
            {
                return std::unexpected(ErrorCode::InvalidArgument);
            }

            header10 = reinterpret_cast<DDS_HEADER_DXT10 const*>(data.data() + sizeof(uint32_t) + sizeof(DDS_HEADER));
        }

        size_t const offset = sizeof(uint32_t) + sizeof(DDS_HEADER) + ((header10 != nullptr) ? sizeof(DDS_HEADER_DXT10) : 0);

        resources = data.subspan(offset);

        return {};
    }

    std::expected<void, ErrorCode> ParseSubresources(
        size_t width,
        size_t height,
        size_t depth,
        size_t mipCount,
        size_t arrayCount,
        size_t planesCount,
        DXGI_FORMAT format,
        size_t maxSize,
        std::span<std::byte> data,
        size_t& outWidth,
        size_t& outHeight,
        size_t& outDepth,
        size_t& outSkipMip,
        std::vector<ImageSurface>& outSurfaces)
    {
        if (data.empty())
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        outWidth = 0;
        outHeight = 0;
        outDepth = 0;
        outSkipMip = 0;

        size_t numBytes = 0;
        size_t rowBytes = 0;

        outSurfaces.clear();

        std::byte* first = data.data();
        std::byte* last = data.data() + data.size();

        for (size_t indexPlane = 0; indexPlane < planesCount; ++indexPlane)
        {
            std::byte* source = first;

            for (size_t indexArray = 0; indexArray < arrayCount; ++indexArray)
            {
                size_t w = width;
                size_t h = height;
                size_t d = depth;

                for (size_t indexMip = 0; indexMip < mipCount; ++indexMip)
                {
                    if (not GetSurfaceInfo(w, h, format, &numBytes, &rowBytes, nullptr))
                    {
                        return std::unexpected(ErrorCode::InvalidArgument);
                    }

                    if ((numBytes > std::numeric_limits<uint32_t>::max()) or (rowBytes > std::numeric_limits<uint32_t>::max()))
                    {
                        return std::unexpected(ErrorCode::InvalidArgument);
                    }

                    size_t bufferSize = numBytes * d;

                    if ((mipCount <= 1) or (maxSize == 0) or ((w <= maxSize) and (h <= maxSize) and (d <= maxSize)))
                    {
                        if (outWidth == 0)
                        {
                            outWidth = w;
                            outHeight = h;
                            outDepth = d;
                        }

                        ImageSurface surface{
                            .Buffer = source,
                            .BufferSize = bufferSize,
                            .LinePitch = rowBytes,
                            .SlicePitch = numBytes,
                            .Size = {
                                .Width = w,
                                .Height = h,
                                .Depth = d,
                            },
                            .MipLevel = indexMip,
                        };

                        AdjustPlanarResource(format, height, indexPlane, surface);

                        outSurfaces.emplace_back(surface);
                    }
                    else if (indexArray == 0)
                    {
                        ++outSkipMip;
                    }

                    if ((source + bufferSize) > last)
                    {
                        // End of file.
                        return std::unexpected(ErrorCode::EndOfFile);
                    }

                    source += bufferSize;

                    w = w >> 1;
                    h = h >> 1;
                    d = d >> 1;

                    if (w == 0)
                    {
                        w = 1;
                    }

                    if (h == 0)
                    {
                        h = 1;
                    }

                    if (d == 0)
                    {
                        d = 1;
                    }
                }
            }

            AE_ASSERT(source <= last);
        }

        if (outSurfaces.empty())
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return {};
    }
}

namespace Anemone::Graphics
{
    std::expected<std::unique_ptr<Image>, ErrorCode> ImageDecoder_DDS(std::span<std::byte> buffer)
    {
        Private::DDS_HEADER const* header{};
        Private::DDS_HEADER_DXT10 const* header10{};
        std::span<std::byte> resources{};

        if (auto ec = Private::ParseFileHeader(buffer, header, header10, resources))
        {
            size_t width = header->Width;
            size_t height = header->Height;
            size_t depth = header->Depth;

            ImageDimension dimension;
            Private::DXGI_FORMAT format = Private::DXGI_FORMAT_UNKNOWN;

            size_t arraySize = 1;
            size_t mipCount = header->MipMapCount;

            if (mipCount == 0)
            {
                mipCount = 1;
            }

            if ((header->PixelFormat.Flags & Private::DDS_PIXELFORMAT_FOURCC) and (header->PixelFormat.FourCC == Private::MakeFourCC('D', 'X', '1', '0')))
            {
                if (header10 == nullptr)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                arraySize = header10->ArraySize;

                if (arraySize == 0)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                switch (header10->Format) // NOLINT(clang-diagnostic-switch-enum)
                {
                case Private::DXGI_FORMAT_NV12:
                case Private::DXGI_FORMAT_P010:
                case Private::DXGI_FORMAT_P016:
                case Private::DXGI_FORMAT_420_OPAQUE:
                    if (header10->ResourceDimension != Private::DDS_RESOURCE_DIMENSION_TEXTURE_2D)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }

                    if (((width % 2) != 0) or ((height % 2) != 0))
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }

                    break;

                case Private::DXGI_FORMAT_YUY2:
                case Private::DXGI_FORMAT_Y210:
                case Private::DXGI_FORMAT_Y216:
                case Private::DXGI_FORMAT_P208:
                    if ((width % 2) != 0)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }
                    break;

                case Private::DXGI_FORMAT_NV11:
                    if ((width % 4) != 0)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }
                    break;

                case Private::DXGI_FORMAT_AI44:
                case Private::DXGI_FORMAT_IA44:
                case Private::DXGI_FORMAT_P8:
                case Private::DXGI_FORMAT_A8P8:
                    return std::unexpected(ErrorCode::NotSupported);

                case Private::DXGI_FORMAT_V208:
                    if ((header10->ResourceDimension != Private::DDS_RESOURCE_DIMENSION_TEXTURE_2D))
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }

                    if ((height % 2) != 0)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }
                    break;

                default:
                    if (Private::GetBitsPerPixel(header10->Format) == 0)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }
                }

                format = header10->Format;

                switch (header10->ResourceDimension)
                {
                case Private::DDS_RESOURCE_DIMENSION_TEXTURE_1D:
                    if ((header->Flags & Private::DDS_HEADER_HEIGHT) and (height != 1))
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }
                    height = depth = 1;
                    dimension = ImageDimension::Texture1D;
                    break;

                case Private::DDS_RESOURCE_DIMENSION_TEXTURE_2D:
                    if (header10->MiscFlags & Private::DDS_RESOURCE_MISC_FLAG_TEXTURE_CUBE)
                    {
                        arraySize *= 6;
                        dimension = ImageDimension::TextureCube;
                    }
                    else
                    {
                        dimension = ImageDimension::Texture2D;
                    }

                    depth = 1;

                    break;

                case Private::DDS_RESOURCE_DIMENSION_TEXTURE_3D:
                    if (not(header->Flags & Private::DDS_HEADER_VOLUME))
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }

                    if (arraySize > 1)
                    {
                        return std::unexpected(ErrorCode::NotSupported);
                    }

                    dimension = ImageDimension::Texture3D;
                    break;

                default:
                    return std::unexpected(ErrorCode::NotSupported);
                }
            }
            else
            {
                format = Private::GetFormat(header->PixelFormat);

                if (format == Private::DXGI_FORMAT_UNKNOWN)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (header->Flags & Private::DDS_HEADER_VOLUME)
                {
                    dimension = ImageDimension::Texture3D;
                }
                else
                {
                    if (header->Cubemap & Private::DDS_CUBEMAP)
                    {
                        if ((header->Cubemap & Private::DDS_CUBEMAP_ALLFACES) != Private::DDS_CUBEMAP_ALLFACES)
                        {
                            return std::unexpected(ErrorCode::NotSupported);
                        }

                        arraySize = 6;
                        dimension = ImageDimension::TextureCube;
                    }
                    else
                    {
                        dimension = ImageDimension::Texture2D;
                    }

                    depth = 1;
                }

                if (Private::GetBitsPerPixel(format) == 0)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }
            }

            if (mipCount > Private::DDS_MAX_MIP_LEVELS)
            {
                return std::unexpected(ErrorCode::NotSupported);
            }

            switch (dimension) // NOLINT(clang-diagnostic-switch-enum)
            {
            case ImageDimension::Texture1D:
                if (arraySize > Private::DDS_TEXTURE_1D_ARRAY_AXIS_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (width > Private::DDS_TEXTURE_1D_U_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }
                break;

            case ImageDimension::Texture2D:
                if (arraySize > Private::DDS_TEXTURE_2D_ARRAY_AXIS_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (width > Private::DDS_TEXTURE_2D_UV_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (height > Private::DDS_TEXTURE_2D_UV_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }
                break;

            case ImageDimension::TextureCube:
                if (arraySize > Private::DDS_TEXTURE_2D_ARRAY_AXIS_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (width > Private::DDS_TEXTURE_CUBE_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (height > Private::DDS_TEXTURE_CUBE_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (width != height)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }
                break;

            case ImageDimension::Texture3D:
                if (arraySize > 1)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (width > Private::DDS_TEXTURE_3D_UVW_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (height > Private::DDS_TEXTURE_3D_UVW_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }

                if (depth > Private::DDS_TEXTURE_3D_UVW_DIMENSION)
                {
                    return std::unexpected(ErrorCode::NotSupported);
                }
                break;

            default:
                return std::unexpected(ErrorCode::NotSupported);
            }

            // TODO: Determine number of planes for given format.
            constexpr size_t planesCount = 1;

            size_t resourcesCount = (dimension == ImageDimension::Texture3D) ? 1 : arraySize;
            resourcesCount *= mipCount;
            resourcesCount *= planesCount;

            if (resourcesCount > Private::DDS_MAX_SUBRESOURCES)
            {
                return std::unexpected(ErrorCode::NotSupported);
            }

            size_t maxSize = (dimension == ImageDimension::Texture3D)
                ? Private::DDS_TEXTURE_3D_UVW_DIMENSION
                : Private::DDS_TEXTURE_2D_UV_DIMENSION;

            size_t skipMips = 0;
            size_t outWidth = 0;
            size_t outHeight = 0;
            size_t outDepth = 0;

            std::vector<ImageSurface> subresources{};

            if (auto rc = Private::ParseSubresources(
                    width,
                    height,
                    depth,
                    mipCount,
                    arraySize,
                    planesCount,
                    format,
                    maxSize,
                    resources,
                    outWidth,
                    outHeight,
                    outDepth,
                    skipMips,
                    subresources))
            {
                auto result = std::make_unique<Image>(
                    ImageSize{
                        outWidth,
                        outHeight,
                        outDepth,
                    },
                    mipCount,
                    arraySize,
                    dimension,
                    Private::PixelFormatFromDxgi(format),
                    Private::GetAlphaMode(header, header10));

                size_t index = 0;
                std::span<ImageSurface> imageSurfaces = result->GetSubresources();
                AE_ASSERT(imageSurfaces.size() == subresources.size());

                for (ImageSurface const& surface : subresources)
                {
                    // FIXME: This API should be improved to
                    AE_ASSERT(imageSurfaces[index].Size.Width == surface.Size.Width);
                    AE_ASSERT(imageSurfaces[index].Size.Height == surface.Size.Height);
                    AE_ASSERT(imageSurfaces[index].Size.Depth == surface.Size.Depth);

                    AE_ASSERT(imageSurfaces[index].BufferSize == surface.BufferSize);
                    AE_ASSERT(imageSurfaces[index].LinePitch == surface.LinePitch);
                    AE_ASSERT(imageSurfaces[index].SlicePitch == surface.SlicePitch);
                    AE_ASSERT(imageSurfaces[index].MipLevel == surface.MipLevel);

                    std::memcpy(imageSurfaces[index].Buffer, surface.Buffer, surface.BufferSize);
                    ++index;
                }

                return result;
            }
            else
            {
                return std::unexpected(rc.error());
            }
        }
        else
        {
            return std::unexpected(ec.error());
        }
    }

    std::expected<void, ErrorCode> ImageEncoder_DDS(Storage::DataWriter& writer, Image const& image)
    {
        size_t const mipCount = image.GetMipMapCount();
        if ((mipCount == 0) or (mipCount > Private::DDS_MAX_MIP_LEVELS))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        size_t arrayCount = image.GetArrayCount();
        if (arrayCount == 0)
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        ImageDimension const dimension = image.GetDimension();

        Private::DXGI_FORMAT const format = Private::PixelFormatToDxgi(image.GetFormat());

        ImageSize const size = image.GetSize();

        if ((size.Width >= static_cast<size_t>(std::numeric_limits<int32_t>::max())) or
            (size.Height >= static_cast<size_t>(std::numeric_limits<int32_t>::max())) or
            (size.Depth >= static_cast<size_t>(std::numeric_limits<int32_t>::max())))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        uint32_t ddsResourceDimension;
        uint32_t ddsWidth = static_cast<uint32_t>(size.Width);
        uint32_t ddsHeight = static_cast<uint32_t>(size.Height);
        uint32_t ddsDepth = static_cast<uint32_t>(size.Depth);

        uint32_t ddsHeaderCubemap = 0;
        uint32_t ddsHeaderSurface = Private::DDS_SURFACE_TEXTURE;
        uint32_t ddsHeaderFlags = Private::DDS_HEADER_CAPS | Private::DDS_HEADER_PIXELFORMAT;

        if (mipCount > 1)
        {
            ddsHeaderFlags |= Private::DDS_HEADER_MIPMAP;
            ddsHeaderSurface |= Private::DDS_SURFACE_MIPMAP;
        }

        switch (dimension)
        {
        case ImageDimension::Texture1D:
            ddsResourceDimension = Private::DDS_RESOURCE_DIMENSION_TEXTURE_1D;
            AE_ASSERT(ddsHeight == 1);
            AE_ASSERT(ddsDepth == 1);
            ddsHeight = 1;
            ddsWidth = 1;
            ddsHeaderFlags |= Private::DDS_HEADER_WIDTH;
            break;

        case ImageDimension::Texture2D:
            ddsResourceDimension = Private::DDS_RESOURCE_DIMENSION_TEXTURE_2D;
            AE_ASSERT(ddsDepth == 1);
            ddsDepth = 1;
            ddsHeaderFlags |= Private::DDS_HEADER_WIDTH | Private::DDS_HEADER_HEIGHT;
            break;

        case ImageDimension::TextureCube:
            ddsResourceDimension = Private::DDS_RESOURCE_DIMENSION_TEXTURE_2D;
            AE_ASSERT(ddsDepth == 1);
            ddsDepth = 1;
            ddsHeaderFlags |= Private::DDS_HEADER_WIDTH | Private::DDS_HEADER_HEIGHT;
            ddsHeaderSurface |= Private::DDS_SURFACE_CUBEMAP;
            ddsHeaderCubemap |= Private::DDS_CUBEMAP_ALLFACES;

            AE_ASSERT(ddsWidth == ddsHeight);

            if (ddsWidth != ddsHeight)
            {
                return std::unexpected(ErrorCode::InvalidArgument);
            }

            if ((arrayCount % 6) != 0)
            {
                return std::unexpected(ErrorCode::InvalidArgument);
            }

            arrayCount /= 6;

            break;

        case ImageDimension::Texture3D:
            ddsResourceDimension = Private::DDS_RESOURCE_DIMENSION_TEXTURE_3D;
            AE_ASSERT(arrayCount == 1);
            if (arrayCount != 1)
            {
                return std::unexpected(ErrorCode::InvalidArgument);
            }
            ddsHeaderFlags |= Private::DDS_HEADER_WIDTH | Private::DDS_HEADER_HEIGHT | Private::DDS_HEADER_VOLUME;
            ddsHeaderCubemap |= Private::DDS_CUBEMAP_VOLUME;
            break;

        case ImageDimension::Unknown:
        default:
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        uint32_t ddsResourceFlags = IsCube(dimension) ? Private::DDS_RESOURCE_MISC_FLAG_TEXTURE_CUBE : 0;

        Private::DDS_HEADER header{
            .Size = sizeof(header),
            .Flags = ddsHeaderFlags,
            .Height = ddsHeight,
            .Width = ddsWidth,
            .Depth = ddsDepth,
            .MipMapCount = static_cast<uint32_t>(mipCount),
            .PixelFormat = Private::DDS_PIXELFORMAT{
                .Size = sizeof(Private::DDS_PIXELFORMAT),
                .Flags = Private::DDS_PIXELFORMAT_FOURCC,
                .FourCC = Private::MakeFourCC('D', 'X', '1', '0'),
                .RGBBitCount = 0,
                .RBitMask = 0,
                .GBitMask = 0,
                .BBitMask = 0,
                .ABitMask = 0,
            },
            .Surface = ddsHeaderSurface,
            .Cubemap = ddsHeaderCubemap,
        };

        Private::DDS_HEADER_DXT10 header10{
            .Format = format,
            .ResourceDimension = ddsResourceDimension,
            .MiscFlags = ddsResourceFlags,
            .ArraySize = static_cast<uint32_t>(arrayCount),
            .MiscFlags2 = Private::ConvertAlphaMode_DXT10(image.GetAlphaMode()),
        };

        uint32_t signature = Private::DDS_HEADER_SIGNATURE;

        if (not writer.Write(std::as_bytes(std::span(&signature, 1))))
        {
            return std::unexpected(ErrorCode::IoError);
        }

        if (not writer.Write(std::as_bytes(std::span{&header, 1})))
        {
            return std::unexpected(ErrorCode::IoError);
        }

        if (not writer.Write(std::as_bytes(std::span{&header10, 1})))
        {
            return std::unexpected(ErrorCode::IoError);
        }

        for (ImageSurface const& surface : image.GetSubresources())
        {
            if (not writer.Write(std::span{surface.Buffer, surface.BufferSize}))
            {
                return std::unexpected(ErrorCode::IoError);
            }
        }

        return {};
    }
}
