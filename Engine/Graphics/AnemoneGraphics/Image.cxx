#include <AnemoneGraphics/Image.hxx>

#include "AnemoneRuntime/Diagnostic/Assert.hxx"

namespace Anemone::Graphics
{
    Image::Image(
        ImageSize size,
        size_t mip_count,
        size_t array_count,
        ImageDimension dimension,
        PixelFormat format,
        ImageAlphaMode alpha_mode)
        : _mip_count{mip_count}
        , _array_count{array_count}
        , _dimension{dimension}
        , _format{format}
        , _alpha_mode{alpha_mode}
    {
        AE_ASSERT(_array_count >= 1);

        this->_size = ComputeSurfaceSize(format, size, 0);

        if (this->_mip_count == 0)
        {
            this->_mip_count = ComputeMipMapLevels(format, size);
        }

        if (IsCube(dimension))
        {
            // TODO: Determine if image should explicitly provide API with expected cube face count.
            AE_ASSERT((this->_array_count % 6) == 0);
        }

        this->_subresources_count = this->_array_count * this->_mip_count;

        this->_subresources = std::make_unique<ImageSurface[]>(this->_subresources_count);

        size_t index = 0;

        size_t num_bytes = 0;
        size_t row_bytes = 0;
        size_t row_count = 0;

        for (size_t indexArray = 0; indexArray < this->_array_count; ++indexArray)
        {
            size_t w = this->_size.Width;
            size_t h = this->_size.Height;
            size_t d = this->_size.Depth;

            for (size_t indexMip = 0; indexMip < this->_mip_count; ++indexMip)
            {

                GetSurfaceInfo(
                    this->_format,
                    w,
                    h,
                    num_bytes,
                    row_bytes,
                    row_count);

                AE_ASSERT(index < this->_subresources_count);

                this->_subresources[index].LinePitch = row_bytes;
                this->_subresources[index].SlicePitch = num_bytes;
                this->_subresources[index].Buffer = nullptr;
                this->_subresources[index].BufferSize = num_bytes * d;
                this->_subresources[index].MipLevel = indexMip;
                this->_subresources[index].Size = ImageSize{w, h, d};

                [[maybe_unused]] size_t const subsize = GetStorageSize(
                    this->_format,
                    w,
                    h,
                    d,
                    0);

                AE_ASSERT(subsize == this->_subresources[index].BufferSize);

                ++index;

                w = std::max<size_t>(1, w >> 1);
                h = std::max<size_t>(1, h >> 1);
                d = std::max<size_t>(1, d >> 1);
            }
        }

        AE_ASSERT(index == this->_subresources_count);

        constexpr size_t scanline_alignment = 16;

        size_t total_size = 0;

        for (size_t i = 0; i < this->_subresources_count; ++i)
        {
            total_size += Bitwise::AlignUp<size_t>(this->_subresources[i].BufferSize, scanline_alignment);
        }

        AE_ASSERT(total_size != 0);

        this->_buffer_size = total_size;
        this->_buffer = std::make_unique<std::byte[]>(this->_buffer_size);

        std::byte* buffer = this->_buffer.get();

        for (size_t i = 0; i < this->_subresources_count; ++i)
        {
            this->_subresources[i].Buffer = buffer;

            buffer += Bitwise::AlignUp(this->_subresources[i].BufferSize, scanline_alignment);
        }
    }

    std::unique_ptr<Image> Image::Create1D(
        PixelFormat format,
        size_t width,
        size_t mipmap_count,
        size_t array_count,
        ImageAlphaMode alphaMode)
    {
        return std::make_unique<Image>(
            ImageSize{width, 1, 1},
            mipmap_count,
            array_count,
            ImageDimension::Texture1D,
            format,
            alphaMode);
    }

    std::unique_ptr<Image> Image::Create2D(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t mipmap_count,
        size_t array_count,
        ImageAlphaMode alphaMode)
    {
        return std::make_unique<Image>(
            ImageSize{width, height, 1},
            mipmap_count,
            array_count,
            ImageDimension::Texture2D,
            format,
            alphaMode);
    }

    std::unique_ptr<Image> Image::Create3D(
        PixelFormat format,
        size_t width,
        size_t height,
        size_t depth,
        size_t mipmap_count,
        size_t array_count,
        ImageAlphaMode alphaMode)
    {
        return std::make_unique<Image>(
            ImageSize{width, height, depth},
            mipmap_count,
            array_count,
            ImageDimension::Texture3D,
            format,
            alphaMode);
    }

    std::unique_ptr<Image> Image::CreateCube(
        PixelFormat format,
        size_t size,
        size_t mipmap_count,
        size_t array_count,
        ImageAlphaMode alphaMode)
    {
        return std::make_unique<Image>(
            ImageSize{size, size, 1},
            mipmap_count,
            array_count,
            ImageDimension::TextureCube,
            format,
            alphaMode);
    }

    std::optional<ImageSurface> Image::GetSubresource(
        size_t arrayIndex,
        size_t mipmapIndex)
    {
        if ((arrayIndex >= this->_array_count) or (mipmapIndex >= this->_mip_count))
        {
            return {};
        }

        if (IsCube(this->_dimension))
        {
            return {};
        }

        size_t const index = ComputeSubresourceIndex(mipmapIndex, arrayIndex, this->_mip_count);

        if (index < this->_subresources_count)
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
        if (not IsCube(this->_dimension))
        {
            return {};
        }

        if ((arrayIndex >= this->_array_count) or (mipmapIndex >= this->_mip_count))
        {
            return {};
        }

        AE_ASSERT((this->_subresources_count % 6) == 0);

        size_t const cubeFaceIndex = static_cast<uint32_t>(cubeFace);

        AE_ASSERT(cubeFaceIndex < 6);

        size_t const index = ComputeSubresourceIndex(mipmapIndex, (arrayIndex * 6) + cubeFaceIndex, this->_mip_count);

        if (index < this->_subresources_count)
        {
            return this->_subresources[index];
        }

        return {};
    }
}
