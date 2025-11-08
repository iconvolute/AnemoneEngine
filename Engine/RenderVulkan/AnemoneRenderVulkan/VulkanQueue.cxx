#include "AnemoneRenderVulkan/VulkanQueue.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"

namespace Anemone
{
    VulkanQueue::VulkanQueue(VulkanDevice* device, uint32_t familyIndex, VulkanQueueType queueType)
        : _device{device}
        , _queue{VK_NULL_HANDLE}
        , _familyIndex{familyIndex}
        , _queueIndex{0}
        , _queueType{queueType}
        , _accessFlags{}
    {
        vkGetDeviceQueue(
            this->_device->_logicalDevice,
            this->_familyIndex,
            this->_queueIndex,
            &this->_queue);

        const VkQueueFamilyProperties& properties = this->_device->m_queueFamilyProperties[this->_familyIndex];

        this->_pipelineStageFlags =
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_HOST_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        this->_accessFlags =
            VK_ACCESS_HOST_READ_BIT |
            VK_ACCESS_HOST_WRITE_BIT |
            VK_ACCESS_MEMORY_READ_BIT |
            VK_ACCESS_MEMORY_WRITE_BIT;

        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            this->_pipelineStageFlags |=
                VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
                VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
                VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                VK_PIPELINE_STAGE_TRANSFER_BIT |
                VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

            this->_accessFlags |=
                VK_ACCESS_TRANSFER_READ_BIT |
                VK_ACCESS_TRANSFER_WRITE_BIT |
                VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
                VK_ACCESS_UNIFORM_READ_BIT |
                VK_ACCESS_SHADER_READ_BIT |
                VK_ACCESS_SHADER_WRITE_BIT |
                VK_ACCESS_INDEX_READ_BIT |
                VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
                VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            if (this->_device->m_physicalDeviceExtensions.GeometryShader)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
            }

            if (this->_device->m_physicalDeviceExtensions.FragmentShadingRate)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

                this->_accessFlags |=
                    VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
            }

            if (this->_device->m_physicalDeviceExtensions.FragmentDensityMap)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;

                this->_accessFlags |=
                    VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
            }
            if (this->_device->m_physicalDeviceExtensions.MeshShader)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT |
                    VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
            }
        }

        if (properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            this->_pipelineStageFlags |=
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
                VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
                VK_PIPELINE_STAGE_TRANSFER_BIT;

            this->_accessFlags |=
                VK_ACCESS_TRANSFER_READ_BIT |
                VK_ACCESS_TRANSFER_WRITE_BIT |
                VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
                VK_ACCESS_UNIFORM_READ_BIT |
                VK_ACCESS_SHADER_READ_BIT |
                VK_ACCESS_SHADER_WRITE_BIT;

            if (this->_device->m_physicalDeviceExtensions.AccelerationStructure)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

                this->_accessFlags |=
                    VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR |
                    VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            }

            if (this->_device->m_physicalDeviceExtensions.RayTracingPipeline)
            {
                this->_pipelineStageFlags |=
                    VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
            }
        }

        if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            this->_pipelineStageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            this->_accessFlags |= VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
    }

    VulkanQueue::~VulkanQueue()
    {
    }

    void VulkanQueue::Submit(VulkanCommandBuffer& commandBuffer)
    {
        (void)commandBuffer;
    }
}
