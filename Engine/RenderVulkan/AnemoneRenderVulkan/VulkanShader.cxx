#include "AnemoneRenderVulkan/VulkanShader.hxx"
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#include "AnemoneRenderVulkan/VulkanCpuAllocator.hxx"

namespace Anemone
{
    VulkanVertexShader::~VulkanVertexShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanTessellationControlShader::~VulkanTessellationControlShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanTessellationEvaluationShader::~VulkanTessellationEvaluationShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanGeometryShader::~VulkanGeometryShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanFragmentShader::~VulkanFragmentShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanMeshShader::~VulkanMeshShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }

    VulkanTaskShader::~VulkanTaskShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }
}
