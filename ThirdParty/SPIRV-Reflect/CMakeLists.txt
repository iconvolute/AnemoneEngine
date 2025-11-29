# C:\VulkanSDK\1.4.328.1\Source\SPIRV-Reflect
anemone_add_sdk(SdkSpirvReflect STATIC)

target_include_directories(SdkSpirvReflect
    PUBLIC
        ${Vulkan_INCLUDE_DIRS}

)

target_compile_definitions(SdkSpirvReflect
    PUBLIC
        SPIRV_REFLECT_USE_SYSTEM_SPIRV_H
)

target_sources(SdkSpirvReflect
    PRIVATE
        "spirv_reflect.c"
)
