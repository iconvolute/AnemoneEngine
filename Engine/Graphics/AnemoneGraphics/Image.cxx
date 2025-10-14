#include "AnemoneGraphics/Image.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::Graphics
{
    Image::Image(
        ImageSize size,
        size_t mipCount,
        size_t arrayCount,
        ImageDimension dimension,
        PixelFormat format,
        ImageAlphaMode alphaMode)
        : _mipCount{mipCount}
        , _arrayCount{arrayCount}
        , _dimension{dimension}
        , _format{format}
        , _alphaMode{alphaMode}
    {
        AE_ASSERT(_arrayCount >= 1);

        this->_size = ComputeSurfaceSize(format, size, 0);

        if (this->_mipCount == 0)
        {
            this->_mipCount = ComputeMipMapLevels(format, size);
        }

        if (dimension == ImageDimension::TextureCube)
        {
            // TODO: Determine if image should explicitly provide API with expected cube face count.
            AE_ASSERT((this->_arrayCount % 6) == 0);
        }

        this->_subresourcesCount = this->_arrayCount * this->_mipCount;

        this->_subresources = std::make_unique<ImageSurface[]>(this->_subresourcesCount);

        size_t index = 0;

        size_t numBytes = 0;
        size_t rowBytes = 0;
        size_t rows = 0;

        for (size_t indexArray = 0; indexArray < this->_arrayCount; ++indexArray)
        {
            size_t w = this->_size.Width;
            size_t h = this->_size.Height;
            size_t d = this->_size.Depth;

            for (size_t indexMip = 0; indexMip < this->_mipCount; ++indexMip)
            {

                GetSurfaceInfo(
                    this->_format,
                    w,
                    h,
                    numBytes,
                    rowBytes,
                    rows);

                AE_ASSERT(index < this->_subresourcesCount);

                this->_subresources[index].LinePitch = rowBytes;
                this->_subresources[index].SlicePitch = numBytes;
                this->_subresources[index].Buffer = nullptr;
                this->_subresources[index].BufferSize = numBytes * d;
                this->_subresources[index].MipLevel = indexMip;
                this->_subresources[index].Size = ImageSize{w, h, d};

                AE_ASSERT(GetStorageSize(this->_format, w, h, d, 0) == this->_subresources[index].BufferSize);
                ++index;

                w = std::max<size_t>(1, w >> 1);
                h = std::max<size_t>(1, h >> 1);
                d = std::max<size_t>(1, d >> 1);
            }
        }

        AE_ASSERT(index == this->_subresourcesCount);

        constexpr size_t scanlineAlignment = 16;

        size_t totalSize = 0;

        for (size_t i = 0; i < this->_subresourcesCount; ++i)
        {
            totalSize += Bitwise::AlignUp<size_t>(this->_subresources[i].BufferSize, scanlineAlignment);
        }

        AE_ASSERT(totalSize != 0);

        this->_bufferSize = totalSize;
        this->_buffer = std::make_unique<std::byte[]>(this->_bufferSize);

        std::byte* buffer = this->_buffer.get();

        for (size_t i = 0; i < this->_subresourcesCount; ++i)
        {
            this->_subresources[i].Buffer = buffer;

            buffer += Bitwise::AlignUp(this->_subresources[i].BufferSize, scanlineAlignment);
        }
    }

    Reference<Image> Image::Create1D(
        PixelFormat format,
        size_t width,
        size_t mipmapCount,
        size_t arrayCount,
        ImageAlphaMode alphaMode)
    {
        return MakeReference<Image>(
            ImageSize{width, 1, 1},
            mipmapCount,
            arrayCount,
            ImageDimension::Texture1D,
            format,
            alphaMode);
    }

    Reference<Image> Image::Create2D(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t mipmapCount,
        size_t arrayCount,
        ImageAlphaMode alphaMode)
    {
        return MakeReference<Image>(
            ImageSize{width, height, 1},
            mipmapCount,
            arrayCount,
            ImageDimension::Texture2D,
            format,
            alphaMode);
    }

    Reference<Image> Image::Create3D(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t depth,
        size_t mipmapCount,
        size_t arrayCount,
        ImageAlphaMode alphaMode)
    {
        return MakeReference<Image>(
            ImageSize{width, height, depth},
            mipmapCount,
            arrayCount,
            ImageDimension::Texture3D,
            format,
            alphaMode);
    }

    Reference<Image> Image::CreateCube(
        PixelFormat format,
        size_t size,
        size_t mipmapCount,
        size_t arrayCount,
        ImageAlphaMode alphaMode)
    {
        return MakeReference<Image>(
            ImageSize{size, size, 1},
            mipmapCount,
            arrayCount,
            ImageDimension::TextureCube,
            format,
            alphaMode);
    }

    std::optional<ImageSurface> Image::GetSubresource(
        size_t arrayIndex,
        size_t mipmapIndex)
    {
        if ((arrayIndex >= this->_arrayCount) or (mipmapIndex >= this->_mipCount))
        {
            return {};
        }

        if (this->_dimension == ImageDimension::TextureCube)
        {
            return {};
        }

        size_t const index = ComputeSubresourceIndex(mipmapIndex, arrayIndex, this->_mipCount);

        if (index < this->_subresourcesCount)
        {
            return this->_subresources[index];
        }

        return {};
    }

    std::optional<ImageSurface> Image::GetSubresource(
        CubeFace cubeFace,
        size_t arrayIndex,
        size_t mipmapIndex)
    {
        if (this->_dimension != ImageDimension::TextureCube)
        {
            return {};
        }

        if ((arrayIndex >= this->_arrayCount) or (mipmapIndex >= this->_mipCount))
        {
            return {};
        }

        AE_ASSERT((this->_subresourcesCount % 6) == 0);

        size_t const cubeFaceIndex = static_cast<uint32_t>(cubeFace);

        AE_ASSERT(cubeFaceIndex < 6);

        size_t const index = ComputeSubresourceIndex(mipmapIndex, (arrayIndex * 6) + cubeFaceIndex, this->_mipCount);

        if (index < this->_subresourcesCount)
        {
            return this->_subresources[index];
        }

        return {};
    }
}
