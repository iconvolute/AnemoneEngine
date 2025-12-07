#pragma once
#include "AnemoneRuntime.Base/MemoryBuffer.hxx"
#include "AnemoneRuntime.Storage/BinaryReader.hxx"
#include "AnemoneRuntime.Storage/BinaryWriter.hxx"

namespace Anemone
{
    struct VulkanShaderBlobHeader
    {
    };

    struct VulkanShaderResourceTable
    {
        enum class GlobalSamplerType : uint8_t
        {
            PointClamped,
            PointWrapped,
            BilinearClamped,
            BilinearWrapped,
            TrilinearClamped,
            TrilinearWrapped,
        };

        struct GlobalSampler final
        {
            
        };

        struct Binding final
        {
            uint32_t set{};
            uint32_t binding{};
            uint32_t type{};
        };
    };

    struct VulkanShaderBlob
    {
        Reference<MemoryBuffer> code{};
    };
}
