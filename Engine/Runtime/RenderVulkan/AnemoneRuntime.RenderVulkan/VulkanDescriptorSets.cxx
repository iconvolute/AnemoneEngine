#include "AnemoneRuntime.RenderVulkan/VulkanDescriptorSets.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCpuAllocator.hxx"

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
