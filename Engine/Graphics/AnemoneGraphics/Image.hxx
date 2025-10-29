#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneGraphics/PixelFormat.hxx"
#include "AnemoneBase/Bitwise.hxx"
#include "AnemoneDiagnostics/Error.hxx"
#include "AnemoneStorage/BinaryWriter.hxx"
#include "AnemoneStorage/BinaryReader.hxx"
#include "AnemoneBase/Reference.hxx"

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

    inline ImageSize ComputeSurfaceSize(PixelFormat format, ImageSize const& size, size_t mipLevel)
    {
        uint32_t const minValue = IsCompressed(format) ? 4 : 1;

        return {
            .Width = std::max<size_t>(minValue, size.Width >> mipLevel),
            .Height = std::max<size_t>(minValue, size.Height >> mipLevel),
            .Depth = std::max<size_t>(1, size.Depth >> mipLevel),
        };
    }

    inline size_t SlicePitchBytes(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t depth) noexcept
    {
        size_t const blockSize = GetBlockSize(format);
        if (blockSize != 0)
        {
            size_t const blocksHorz = AlignUp<size_t>(width, 4) / 4;
            size_t const blocksVert = AlignUp<size_t>(height, 4) / 4;
            size_t const blocksCount = blocksHorz * blocksVert;
            return blocksCount * blockSize * depth;
        }
        else
        {
            uint64_t bitsTotal = width;
            bitsTotal *= height;
            bitsTotal *= depth;
            bitsTotal *= GetPixelBits(format);
            size_t const bytesTotal = bitsTotal / 8;
            return bytesTotal;
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
        size_t const bitsPerPixel = GetPixelBits(format);

        // For compressed pixel format, bitsPerPixel == 0.
        return (width * bitsPerPixel) / 8;
    }

    inline size_t RowPitchBytes(
        PixelFormat format,
        uint32_t width) noexcept
    {
        size_t const blockSize = GetBlockSize(format);
        if (blockSize != 0)
        {
            uint32_t const blocksCount = AlignUp<uint32_t>(width, 4) / 4;
            return blocksCount * blockSize;
        }
        else
        {
            size_t const bitsPerPixel = GetPixelBits(format);
            size_t const result = AlignUp<size_t>(width * bitsPerPixel, 8) / 8;
            return result;
        }
    }

    inline void GetSurfaceInfo(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t& outBytes,
        size_t& outRowBytes,
        size_t& outRows) noexcept
    {
        size_t const blockSize = GetBlockSize(format);

        if (blockSize != 0)
        {
            size_t blocksHorz = 0;
            if (width > 0)
            {
                blocksHorz = std::max<size_t>(1, AlignUp<size_t>(width, 4) / 4);
            }

            size_t blocksVert = 0;
            if (height > 0)
            {
                blocksVert = std::max<size_t>(1, AlignUp<size_t>(height, 4) / 4);
            }

            outRowBytes = blocksHorz * blockSize;
            outRows = blocksVert;
        }
        else
        {
            size_t const bpp = GetPixelBits(format);
            outRowBytes = AlignUp<size_t>(width * bpp, 8) / 8;
            outRows = height;
        }

        outBytes = outRowBytes * outRows;
    }

    inline size_t ComputeMipMapLevels(
        PixelFormat format,
        ImageSize size) noexcept
    {
        size_t const minValue = IsCompressed(format) ? 4 : 1;
        size_t levels = 1;

        auto [width, height, depth] = size;

        while ((width > minValue) or (height > minValue) or (depth > 1))
        {
            width = std::max<size_t>(minValue, width >> 1);
            height = std::max<size_t>(minValue, height >> 1);
            depth = std::max<size_t>(1, depth >> 1);

            ++levels;
        }

        return levels;
    }

    class GRAPHICS_API Image final : public ReferenceCounted<Image>
    {
    private:
        ImageSize _size;
        size_t _mipCount;
        size_t _arrayCount;
        ImageDimension _dimension;
        PixelFormat _format;
        ImageAlphaMode _alphaMode;
        size_t _subresourcesCount;
        std::unique_ptr<ImageSurface[]> _subresources;
        size_t _bufferSize;
        std::unique_ptr<std::byte[]> _buffer;

    public:
        Image(
            ImageSize size,
            size_t mipCount,
            size_t arrayCount,
            ImageDimension dimension,
            PixelFormat format,
            ImageAlphaMode alphaMode);

        Image(Image const&) = delete;

        Image(Image&&) noexcept = delete;

        Image& operator=(Image const&) = delete;

        Image& operator=(Image&&) = delete;

        ~Image() = default;

    public:
        static Reference<Image> Create1D(
            PixelFormat format,
            size_t width,
            size_t mipmapCount,
            size_t arrayCount,
            ImageAlphaMode alphaMode);

        static Reference<Image> Create2D(
            PixelFormat format,
            size_t width,
            size_t height,
            size_t mipmapCount,
            size_t arrayCount,
            ImageAlphaMode alphaMode);

        static Reference<Image> Create3D(
            PixelFormat format,
            size_t width,
            size_t height,
            size_t depth,
            size_t mipmapCount,
            size_t arrayCount,
            ImageAlphaMode alphaMode);

        static Reference<Image> CreateCube(
            PixelFormat format,
            size_t size,
            size_t mipmapCount,
            size_t arrayCount,
            ImageAlphaMode alphaMode);

    public:
        constexpr ImageSize GetSize() const
        {
            return this->_size;
        }

        constexpr size_t GetMipMapCount() const
        {
            return this->_mipCount;
        }

        constexpr size_t GetArrayCount() const
        {
            return this->_arrayCount;
        }

        constexpr ImageDimension GetDimension() const
        {
            return this->_dimension;
        }

        constexpr PixelFormat GetFormat() const
        {
            return this->_format;
        }

        constexpr ImageAlphaMode GetAlphaMode() const
        {
            return this->_alphaMode;
        }

    public:
        constexpr std::span<ImageSurface> GetSubresources()
        {
            return {this->_subresources.get(), this->_subresourcesCount};
        }

        constexpr std::span<ImageSurface const> GetSubresources() const
        {
            return {this->_subresources.get(), this->_subresourcesCount};
        }

        constexpr std::optional<ImageSurface> GetSubresource(size_t index)
        {
            if (index < this->_subresourcesCount)
            {
                return this->_subresources[index];
            }

            return {};
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
    GRAPHICS_API std::expected<Reference<Image>, Error> ImageDecoder_DDS(std::span<std::byte> buffer);
    GRAPHICS_API std::expected<void, Error> ImageEncoder_DDS(BinaryWriter& writer, Image const& image);
}
