#pragma once
#include "VulkanUtils.hxx"

#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRenderVulkan/VulkanError.hxx"
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneBase/FNV.hxx"

#include <spirv-tools/libspirv.h>
#include <spirv_cross/spirv_reflect.hpp>
namespace Anemone
{
    constexpr uint32_t Hash(VkDescriptorType type)
    {
        return static_cast<uint32_t>(type) ^ 0x8BFu;
    }

    // The problem: we need to 
    enum class VulkanDescriptorType : uint8_t
    {
        Sampler,
        CombinedImageSampler,
        SampledImage,/*
        VK_DESCRIPTOR_TYPE_SAMPLER = 0,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
        VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV = 1000165000,
        VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM = 1000440000,
        VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM = 1000440001,
        VK_DESCRIPTOR_TYPE_TENSOR_ARM = 1000460000,
        VK_DESCRIPTOR_TYPE_MUTABLE_EXT = 1000351000,
        VK_DESCRIPTOR_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_NV = 1000570000,
        VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
        VK_DESCRIPTOR_TYPE_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
        VK_DESCRIPTOR_TYPE_MAX_ENUM = 0x7FFFFFFF*/
    };

    class VulkanDevice;

    //struct VuilkanDescriptorSetLayoutBindings
    //{
    //    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    //    uint64_t hash{};

    //    static uint64_t ComputeHash(std::span<VkDescriptorSetLayoutBinding const> bindings)
    //    {
    //        return FNV1A64::FromBuffer(std::as_bytes(std::span{bindings}));
    //    }
    //};

    //class VulkanDescriptorSetLayoutProperties
    //{
    //    VulkanDescriptorSetLayoutProperties()
    //    {
    //        VkDescriptorSetLayoutBinding binding{};
    //        //VK_DESCRIPTOR_TYPE_SAMPLER
    //    }
    //};


    //
    // Descriptor set layout describes single GLSL layout(set = N) bindings array.
    //
    // When pipeline layout is created, multiple descriptor set layouts are combined together.
    // Renderer assumes set index represents the frequency of updates:
    // - set = 0 : per draw call
    // - set = 1 : per material
    // - set = 2 : per frame
    // - set = 3 : per application
    // Descriptor sets are allocated from descriptor pools based on descriptor set layouts.
    //
    // Two approaches:
    // - define bindings statically in source code
    //   - bindings must be changed in sync with shader code
    // - read binding from shader reflection data
    //   - bindings are defined in shader code only, so descriptor set layouts are generated automatically
    //   - still need to match the bindings when updating descriptor sets
    //
    // First approach seems simpler for now. However, we still need to compile and verify shaders, so adding reflection
    // information may become necessary in the future.
    //

    class VulkanDescriptorSetLayout : private IntrusiveListNode<VulkanDescriptorSetLayout>
    {
        friend struct IntrusiveList<VulkanDescriptorSetLayout>;

        // store information about layout
        VkDescriptorSetAllocateInfo m_descriptorSetAllocateInfo;

        VkDescriptorSetLayout m_descriptorSetLayout;

        explicit VulkanDescriptorSetLayout()
        {
            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
            RHIVulkanInitialize(descriptorSetLayoutCreateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
            descriptorSetLayoutCreateInfo.flags = 0;
            //uint32_t bindingCount;
            //const VkDescriptorSetLayoutBinding* pBindings;

            //VkDescriptorSetLayoutCreateFlags flags;
            //uint32_t bindingCount;
            //const VkDescriptorSetLayoutBinding* pBindings;
        }
    };

    class VulkanDescriptorSet : private IntrusiveListNode<VulkanDescriptorSet>
    {
        friend struct IntrusiveList<VulkanDescriptorSet>;

        VkDescriptorSet m_descriptorSet;

        explicit VulkanDescriptorSet()
        {
            //vkdescriptorsetcreateinf
        }
    };

    class VulkanDescriptorPool : private IntrusiveListNode<VulkanDescriptorPool>
    {
        friend struct IntrusiveList<VulkanDescriptorPool>;

    private:
        VulkanDevice* m_device{};
        VulkanDescriptorSetLayout const* m_layout{};

        VkDescriptorPool m_descriptorPool{};

    public:
        VulkanDescriptorPool(VulkanDevice& device, VulkanDescriptorSetLayout const& layout, uint32_t maxAllocations);
        ~VulkanDescriptorPool();
    };

}
