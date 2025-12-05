# https://github.com/catchorg/Catch2/releases/tag/v3.10.0
anemone_add_target(
    STATIC SDK
    NAME
        SdkCatch2
)

if(MSVC)
# TODO: Remove this once Catch2 is fixed
target_compile_definitions(SdkCatch2 PUBLIC "CATCH_CONFIG_NO_MSVC_UMUL128")
endif()

target_compile_definitions(SdkCatch2 PUBLIC "CATCH_AMALGAMATED_CUSTOM_MAIN")

add_subdirectory("source")
add_subdirectory("include")
