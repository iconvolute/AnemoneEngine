#pragma once
#include "AnemoneRender/Device.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanDevice;

    enum class VulkanQueueType
    {
        Graphics,
        Compute,
        Transfer,
    };

    class VulkanQueue : public ReferenceCounted<VulkanQueue>
    {
    public:
        VulkanQueue(VulkanDevice* device, uint32_t familyIndex, VulkanQueueType queueType);
        ~VulkanQueue();

    public:
        VulkanDevice* _device{};
        VkQueue _queue{};
        uint32_t _familyIndex{};
        uint32_t _queueIndex{};
        VulkanQueueType _queueType{};
        VkAccessFlags _accessFlags{};
        VkPipelineStageFlags _pipelineStageFlags{};
    };
}
