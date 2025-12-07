#include "AnemoneRuntime.RenderVulkan/VulkanVertexDeclaration.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"
#include "AnemoneRuntime.Base/XXHash64.hxx"

namespace Anemone
{
    static_assert(std::has_unique_object_representations_v<VkVertexInputAttributeDescription>);
    static_assert(std::has_unique_object_representations_v<VkVertexInputBindingDescription>);

    inline VkVertexInputRate ToVulkanInputRate(GpuVertexElementFrequency frequency)
    {
        if (frequency == GpuVertexElementFrequency::PerVertex)
        {
            return VK_VERTEX_INPUT_RATE_VERTEX;
        }

        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }

    inline VkFormat ToVulkanVertexElementType(GpuVertexElementFormat format)
    {
        switch (format)
        {
        case GpuVertexElementFormat::None:
            break;
        case GpuVertexElementFormat::Float1:
            return VK_FORMAT_R32_SFLOAT;
        case GpuVertexElementFormat::Float2:
            return VK_FORMAT_R32G32_SFLOAT;
        case GpuVertexElementFormat::Float3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case GpuVertexElementFormat::Float4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GpuVertexElementFormat::Half2:
            return VK_FORMAT_R16G16_SFLOAT;
        case GpuVertexElementFormat::Half4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        }

        AE_PANIC("Unsupported vertex element format");
        return VK_FORMAT_UNDEFINED;
    }

    static_assert(std::has_unique_object_representations_v<GpuVertexAttribute>);

    GpuVertexDeclarationRef VulkanDevice::CreateVertexDeclaration(GpuVertexDeclarationInitializer const& initializer)
    {
        AE_ASSERT(initializer.attributes.size() < GpuMaxVertexElementCount);
        GpuVertexAttribute attributes[GpuMaxVertexElementCount];
        auto end = std::copy(initializer.attributes.begin(), initializer.attributes.end(), attributes);
        std::sort(attributes, end, [](GpuVertexAttribute const& a, GpuVertexAttribute const& b) -> bool
        {
            // Sort by attribute location.
            return a.attribute < b.attribute;
        });

        XXHash64 hasher{};
        hasher.Update(std::as_bytes(std::span{attributes, initializer.attributes.size()}));
        uint64_t const hash = hasher.Finalize();

        if (VulkanVertexDeclaration* cached = this->m_vertexDeclarations.Find([hash](VulkanVertexDeclaration const& declaration)
        {
            return declaration.m_hash == hash;
        }))
        {
            return Reference{cached};
        }

        Reference<VulkanVertexDeclaration> result = MakeReference<VulkanVertexDeclaration>();
        result->m_hash = hash;

        // Index of Vulkan binding entry.
        uint16_t currentBindingIndex = 0;

        // Reverse map from attribute binding to Vulkan binding index.
        VkVertexInputBindingDescription* bindingMap[GpuMaxVertexElementCount]{};

        // Binding information should be duplicated in initializer data.
        for (auto const& element : initializer.attributes)
        {
            AE_ASSERT(currentBindingIndex < GpuMaxVertexElementCount);

            if (VkVertexInputBindingDescription*& bindingDescription = bindingMap[element.binding])
            {
                // Ensure that existing binding matches.
                AE_ASSERT(bindingDescription->binding == element.binding);
                AE_ASSERT(bindingDescription->stride == element.stride);
                AE_ASSERT(bindingDescription->inputRate == ToVulkanInputRate(element.frequency));
            }
            else
            {
                // Allocate new binding description.
                bindingDescription = &result->m_bindings[currentBindingIndex++];
                bindingDescription->binding = element.binding;
                bindingDescription->stride = element.stride;
                bindingDescription->inputRate = ToVulkanInputRate(element.frequency);
            }
        }

        result->m_bindingCount = currentBindingIndex;

        // Index of Vulkan attribute entry.
        uint16_t currentAttributeIndex = 0;

        VkVertexInputAttributeDescription* attributeMap[GpuMaxVertexElementCount]{};

        for (auto const& element : initializer.attributes)
        {
            AE_ASSERT(currentAttributeIndex < GpuMaxVertexElementCount);
            AE_ASSERT(element.attribute < GpuMaxVertexElementCount);
            AE_ASSERT(attributeMap[element.attribute] == nullptr);

            VkVertexInputAttributeDescription* attributeDescription = &result->m_attributes[currentAttributeIndex++];

            // Remember where attribute is mapped
            attributeMap[element.attribute] = attributeDescription;

            attributeDescription->location = element.attribute;
            attributeDescription->binding = element.binding;
            attributeDescription->format = ToVulkanVertexElementType(element.format);
            attributeDescription->offset = element.offset;
        }

        result->m_attributeCount = currentAttributeIndex;

        // Remember in cache.
        result->AcquireReference();
        this->m_vertexDeclarations.PushBack(result.Get());

        return result;
    }
}
