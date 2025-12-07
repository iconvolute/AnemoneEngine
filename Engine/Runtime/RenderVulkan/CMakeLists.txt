anemone_add_target(
    MODULE
    NAME
        AnemoneRuntime.RenderVulkan
    ALIAS
        Anemone::Runtime.RenderVulkan
    EXPORT
        "ANEMONE_RUNTIME_RENDER_VULKAN_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime.Base
            AnemoneRuntime.Render
            SdkVolk
            SdkVulkanMemoryAllocator
)

target_sources(AnemoneRuntime.RenderVulkan
    PRIVATE
        "Module.cxx"
)

add_subdirectory("AnemoneRuntime.RenderVulkan")
