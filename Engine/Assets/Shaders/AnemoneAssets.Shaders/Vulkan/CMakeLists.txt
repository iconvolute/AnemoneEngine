target_sources(AnemoneAssets.Shaders
    PRIVATE
        "VulkanShaderCompiler.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "VulkanShaderCompiler.hxx"
)


target_link_libraries(AnemoneAssets.Shaders
    PUBLIC
        AnemoneRuntime.RenderVulkan
        Vulkan::shaderc_combined
        SdkSpirvReflect
)
