target_sources(AnemoneAssets.Shaders
    PRIVATE
        "ShaderCompiler.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "ShaderCompiler.hxx"
)

if (Vulkan_FOUND)
add_subdirectory("Vulkan")
endif()
