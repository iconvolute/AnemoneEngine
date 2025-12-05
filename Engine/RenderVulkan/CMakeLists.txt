anemone_add_target(
    MODULE
    NAME AnemoneRenderVulkan
    ALIAS Anemone::RenderVulkan
    EXPORT "RENDER_VULKAN_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
            AnemoneRender
)

target_link_libraries(AnemoneRenderVulkan
    PUBLIC
        SdkVolk
        SdkVulkanMemoryAllocator
)

add_subdirectory("AnemoneRenderVulkan")
