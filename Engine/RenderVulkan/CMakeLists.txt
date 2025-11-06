anemone_add_module(AnemoneRenderVulkan
    ALIAS Anemone::RenderVulkan
    EXPORT "RENDER_VULKAN_API"
)

target_link_libraries(AnemoneRenderVulkan
    PUBLIC
        AnemoneRuntime
        AnemoneRender
        SdkVolk
        SdkVulkanMemoryAllocator
)

add_subdirectory("AnemoneRenderVulkan")
