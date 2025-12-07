#pragma once
#include "AnemoneRuntime.Render/Gpu.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanHeaders.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanError.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanCpuAllocator.hxx"
#include "AnemoneRuntime.RenderVulkan/VulkanDevice.hxx"

namespace Anemone
{
    class VulkanSemaphore : public ThreadsafeReferenceCounted<VulkanSemaphore>
    {
    private:
        VkSemaphore m_handle{};
        VulkanDevice* m_device{};
        bool m_timeline{};

    public:
        explicit VulkanSemaphore(VulkanDevice& device)
            : m_device{&device}
            , m_timeline{false}
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
            };

            AE_VULKAN_ENSURE(vkCreateSemaphore(
                this->m_device->m_device,
                &semaphoreCreateInfo,
                VulkanCpuAllocator,
                &this->m_handle));
        }

        VulkanSemaphore(VulkanDevice& device, uint64_t initialValue)
            : m_device{&device}
            , m_timeline{true}
        {
            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                .pNext = nullptr,
                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                .initialValue = initialValue,
            };

            VkSemaphoreCreateInfo semaphoreCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                .pNext = &semaphoreTypeCreateInfo,
                .flags = 0,
            };

            AE_VULKAN_ENSURE(vkCreateSemaphore(
                this->m_device->m_device,
                &semaphoreCreateInfo,
                VulkanCpuAllocator,
                &this->m_handle));
        }

        ~VulkanSemaphore()
        {
            vkDestroySemaphore(
                this->m_device->m_device,
                this->m_handle,
                VulkanCpuAllocator);
        }

        VkSemaphore GetHandle() const
        {
            return this->m_handle;
        }

        bool WaitForTimelineValue(uint64_t value, uint64_t timeout) const
        {
            AE_ASSERT(this->m_timeline);
            AE_ASSERT(this->m_handle != VK_NULL_HANDLE);

            VkSemaphoreWaitInfo semaphoreWaitInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                .pNext = nullptr,
                .flags = 0,
                .semaphoreCount = 1,
                .pSemaphores = &this->m_handle,
                .pValues = &value,
            };

            VkResult const result = vkWaitSemaphores(
                this->m_device->m_device,
                &semaphoreWaitInfo,
                timeout);

            if (result == VK_SUCCESS)
            {
                return true;
            }

            if (result == VK_TIMEOUT)
            {
                return false;
            }

            AE_VULKAN_ENSURE(result);
            return false;
        }

        uint64_t GetTimelineValue() const
        {
            uint64_t result{};
            AE_VULKAN_ENSURE(vkGetSemaphoreCounterValue(
                this->m_device->m_device,
                this->m_handle,
                &result));
            return result;
        }
    };
}

namespace Anemone
{
    class VulkanFence : public ThreadsafeReferenceCounted<VulkanFence>
    {
    private:
        VkFence m_handle{};
        VulkanDevice* m_device{};

    public:
        explicit VulkanFence(VulkanDevice& device)
            : m_device{&device}
        {
            VkFenceCreateInfo fenceCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
            };

            AE_VULKAN_ENSURE(vkCreateFence(
                this->m_device->m_device,
                &fenceCreateInfo,
                VulkanCpuAllocator,
                &this->m_handle));
        }

        ~VulkanFence()
        {
            vkDestroyFence(
                this->m_device->m_device,
                this->m_handle,
                VulkanCpuAllocator);
        }

        VkFence GetHandle() const
        {
            return this->m_handle;
        }

        void Wait()
        {
            AE_VULKAN_ENSURE(vkWaitForFences(
                this->m_device->m_device,
                1,
                &this->m_handle,
                VK_TRUE,
                UINT64_MAX));
        }

        void Reset()
        {
            AE_VULKAN_ENSURE(vkResetFences(
                this->m_device->m_device,
                1,
                &this->m_handle));
        }
    };
}
