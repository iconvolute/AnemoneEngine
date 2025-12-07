#pragma once
#include "AnemoneRender/Gpu.hxx"
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"

namespace Anemone
{
    class VulkanTexture : public GpuTexture
    {
    public:
        VkImage m_image;
        VkImageView m_imageView;

        
    };
}
