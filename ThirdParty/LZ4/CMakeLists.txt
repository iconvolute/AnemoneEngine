# https://github.com/lz4/lz4/releases/tag/v1.10.0
anemone_add_target(
    STATIC SDK
    NAME
        SdkLZ4
)

add_subdirectory("source")
add_subdirectory("include")
