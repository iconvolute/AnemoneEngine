#include "AnemoneRuntime.RenderVulkan/VulkanShader.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCpuAllocator.hxx"

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

    VulkanComputeShader::~VulkanComputeShader()
    {
        // TODO: Defer to deletion queues
        vkDestroyShaderModule(
            this->m_device->m_device,
            this->m_handle,
            VulkanCpuAllocator);
    }
}
