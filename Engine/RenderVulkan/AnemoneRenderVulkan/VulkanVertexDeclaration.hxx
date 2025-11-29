#pragma once
#include "AnemoneRender/Gpu.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanVertexDeclaration
        : public GpuVertexDeclaration
        , private IntrusiveListNode<VulkanVertexDeclaration>
    {
        friend struct IntrusiveList<VulkanVertexDeclaration>;

    public:
        uint32_t m_bindingCount;
        uint32_t m_attributeCount;
        uint64_t m_hash;
        VkVertexInputBindingDescription m_bindings[GpuMaxVertexElementCount];
        VkVertexInputAttributeDescription m_attributes[GpuMaxVertexElementCount];
    };
}
