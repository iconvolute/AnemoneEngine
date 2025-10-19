target_sources(AnemoneRenderVulkan
    PRIVATE
        "VulkanDevice.cxx"
        "VulkanError.cxx"
        "VulkanHeaders.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "VulkanDevice.hxx"
        "VulkanError.hxx"
        "VulkanHeaders.hxx"
)
