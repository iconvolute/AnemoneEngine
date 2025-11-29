#include "AnemoneRenderVulkan/VulkanDescriptorSets.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"

namespace Anemone
{
    VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device, VulkanDescriptorSetLayout const& layout, uint32_t maxAllocations)
    {
        (void)device;
        (void)layout;
        (void)maxAllocations;

        //VkDescriptorPoolSize poolSize{
        //.type = ,
        //.descriptorCount = ,
        //};
        //
        //VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{
        //    .sType = ,
        //    .pNext = ,
        //    .flags = ,
        //    .maxSets = ,
        //    .poolSizeCount = ,
        //    .pPoolSizes = ,            
        //};

        
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(
            this->m_device->m_device,
            this->m_descriptorPool,
            VulkanCpuAllocator);
        this->m_descriptorPool = VK_NULL_HANDLE;
    }
}
