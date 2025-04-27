#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneGraphics/PixelFormat.hxx"
#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/Storage/DataWriter.hxx"
#include "AnemoneRuntime/Storage/DataReader.hxx"

#include <cstdint>
#include <cstddef>
#include <memory>
#include <span>
#include <expected>

namespace Anemone::Graphics
{
    enum class CubeFace
    {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    enum class ImageAlphaMode
    {
        Unknown = 0,
        Straight = 1,
        Premultiplied = 2,
        Opaque = 3,
        Custom = 4,
    };

    enum class ImageDimension
    {
        Unknown = 0,
        Texture1D = 1,
        Texture2D = 2,
        Texture3D = 3,
        TextureCube = 4,
    };

    constexpr bool IsCube(ImageDimension dimension)
    {
        return (dimension == ImageDimension::TextureCube);
    }

    struct ImageSize final
    {
        size_t Width;
        size_t Height;
        size_t Depth;
    };

    struct ImageSurface final
    {
        std::byte* Buffer;
        size_t BufferSize;

        size_t LinePitch;
        size_t SlicePitch;

        ImageSize Size;
        size_t MipLevel;

        [[nodiscard]] constexpr std::byte const* GetScanline(size_t line, size_t slice = 0) const
        {
            return this->Buffer + (line * this->LinePitch) + (slice * this->SlicePitch);
        }

        [[nodiscard]] constexpr std::byte* GetScanline(size_t line, size_t slice = 0)
        {
            return this->Buffer + (line * this->LinePitch) + (slice * this->SlicePitch);
        }
    };

    constexpr size_t ComputeSubresourceIndex(size_t indexMip, size_t indexArray, size_t countMip)
    {
        return indexMip + (indexArray * countMip);
    }

    inline ImageSize ComputeSurfaceSize(PixelFormat format, ImageSize const& size, size_t mip_level)
    {
        uint32_t const minValue = IsCompressed(format) ? 4 : 1;

        return {
            .Width = std::max<size_t>(minValue, size.Width >> mip_level),
            .Height = std::max<size_t>(minValue, size.Height >> mip_level),
            .Depth = std::max<size_t>(1, size.Depth >> mip_level),
        };
    }


    inline size_t SlicePitchBytes(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t depth) noexcept
    {
        size_t const block_size = GetBlockSize(format);
        if (block_size != 0)
        {
            size_t const blocks_horz = Bitwise::AlignUp<size_t>(width, 4) / 4;
            size_t const blocks_vert = Bitwise::AlignUp<size_t>(height, 4) / 4;
            size_t const blocks_count = blocks_horz * blocks_vert;
            return blocks_count * block_size * depth;
        }
        else
        {
            uint64_t bits_total = width;
            bits_total *= height;
            bits_total *= depth;
            bits_total *= GetPixelBits(format);
            size_t const bytes_total = bits_total / 8;
            return bytes_total;
        }
    }

    inline size_t GetStorageSize(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t depth,
        size_t mipmaps) noexcept
    {
        size_t const w = std::max<size_t>(1, width >> mipmaps);
        size_t const h = std::max<size_t>(1, height >> mipmaps);
        size_t const d = std::max<size_t>(1, depth >> mipmaps);
        size_t const size = SlicePitchBytes(format, w, h, d);
        return size;
    }

    inline size_t LinePitchBytes(
        PixelFormat format,
        uint32_t width) noexcept
    {
        size_t const bits_per_pixel = GetPixelBits(format);

        // For compressed pixel format, bits_per_pixel == 0.
        return (width * bits_per_pixel) / 8;
    }

    inline size_t RowPitchBytes(
        PixelFormat format,
        uint32_t width) noexcept
    {
        size_t const block_size = GetBlockSize(format);
        if (block_size != 0)
        {
            uint32_t const blocks_count = Bitwise::AlignUp<uint32_t>(width, 4) / 4;
            return blocks_count * block_size;
        }
        else
        {
            size_t const bits_per_pixel = GetPixelBits(format);
            size_t const result = Bitwise::AlignUp<size_t>(width * bits_per_pixel, 8) / 8;
            return result;
        }
    }

    inline void GetSurfaceInfo(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t& out_bytes,
        size_t& out_row_bytes,
        size_t& out_rows) noexcept
    {
        size_t const block_size = GetBlockSize(format);

        if (block_size != 0)
        {
            size_t blocks_horz = 0;
            if (width > 0)
            {
                blocks_horz = std::max<size_t>(1, Bitwise::AlignUp<size_t>(width, 4) / 4);
            }

            size_t blocks_vert = 0;
            if (height > 0)
            {
                blocks_vert = std::max<size_t>(1, Bitwise::AlignUp<size_t>(height, 4) / 4);
            }

            out_row_bytes = blocks_horz * block_size;
            out_rows = blocks_vert;
        }
        else
        {
            size_t const bpp = GetPixelBits(format);
            out_row_bytes = Bitwise::AlignUp<size_t>(width * bpp, 8) / 8;
            out_rows = height;
        }

        out_bytes = out_row_bytes * out_rows;
    }

    inline size_t ComputeMipMapLevels(
        PixelFormat format,
        ImageSize size) noexcept
    {
        size_t const min_value = IsCompressed(format) ? 4 : 1;
        size_t levels = 1;

        auto [width, height, depth] = size;

        while ((width > min_value) or (height > min_value) or (depth > 1))
        {
            width = std::max<size_t>(min_value, width >> 1);
            height = std::max<size_t>(min_value, height >> 1);
            depth = std::max<size_t>(1, depth >> 1);

            ++levels;
        }

        return levels;
    }

    class GRAPHICS_API Image final
    {
    private:
        ImageSize _size;
        size_t _mip_count;
        size_t _array_count;
        ImageDimension _dimension;
        PixelFormat _format;
        ImageAlphaMode _alpha_mode;
        size_t _subresources_count;
        std::unique_ptr<ImageSurface[]> _subresources;
        size_t _buffer_size;
        std::unique_ptr<std::byte[]> _buffer;

    public:
        Image(
            ImageSize size,
            size_t mip_count,
            size_t array_count,
            ImageDimension dimension,
            PixelFormat format,
            ImageAlphaMode alpha_mode);

        Image(Image const&) = delete;
        Image(Image&&) noexcept = default;
        Image& operator=(Image const&) = delete;
        Image& operator=(Image&&) = default;

    public:
        static std::unique_ptr<Image> Create1D(
            PixelFormat format,
            size_t width,
            size_t mipmap_count,
            size_t array_count,
            ImageAlphaMode alphaMode);

        static std::unique_ptr<Image> Create2D(
            PixelFormat format,
            size_t width,
            size_t height,
            size_t mipmap_count,
            size_t array_count,
            ImageAlphaMode alphaMode);

        static std::unique_ptr<Image> Create3D(
            PixelFormat format,
            size_t width,
            size_t height,
            size_t depth,
            size_t mipmap_count,
            size_t array_count,
            ImageAlphaMode alphaMode);

        static std::unique_ptr<Image> CreateCube(
            PixelFormat format,
            size_t size,
            size_t mipmap_count,
            size_t array_count,
            ImageAlphaMode alphaMode);

    public:
        ImageSize GetSize() const
        {
            return this->_size;
        }

        size_t GetMipMapCount() const
        {
            return this->_mip_count;
        }

        size_t GetArrayCount() const
        {
            return this->_array_count;
        }

        ImageDimension GetDimension() const
        {
            return this->_dimension;
        }

        PixelFormat GetFormat() const
        {
            return this->_format;
        }

        ImageAlphaMode GetAlphaMode() const
        {
            return this->_alpha_mode;
        }

    public:
        std::span<ImageSurface> GetSubresources()
        {
            return {this->_subresources.get(), this->_subresources_count};
        }

        std::span<ImageSurface const> GetSubresources() const
        {
            return {this->_subresources.get(), this->_subresources_count};
        }

        std::optional<ImageSurface> GetSubresource(size_t index)
        {
            if (index < this->_subresources_count)
            {
                return this->_subresources[index];
            }

            return std::nullopt;
        }

        std::optional<ImageSurface> GetSubresource(
            size_t arrayIndex,
            size_t mipmapIndex);

        std::optional<ImageSurface> GetSubresource(
            CubeFace cubeFace,
            size_t arrayIndex,
            size_t mipmapIndex);
    };
}

namespace Anemone::Graphics
{
    GRAPHICS_API std::expected<std::unique_ptr<Image>, ErrorCode> ImageDecoder_DDS(std::span<std::byte> buffer);
    GRAPHICS_API std::expected<void, ErrorCode> ImageEncoder_DDS(Storage::DataWriter& writer, Image const& image);
}
