target_sources(AnemoneRenderVulkan
    PRIVATE
        "VulkanDevice.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "VulkanDevice.hxx"
)
