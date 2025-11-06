target_sources(AnemoneRenderVulkan
    PRIVATE
        "Module.cxx"
        "VulkanDevice.cxx"
        "VulkanError.cxx"
        "VulkanHeaders.cxx"
        "VulkanQueue.cxx"
        "VulkanSwapChain.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Module.hxx"
        "VulkanDevice.hxx"
        "VulkanError.hxx"
        "VulkanHeaders.hxx"
        "VulkanQueue.hxx"
        "VulkanSwapChain.hxx"
)
