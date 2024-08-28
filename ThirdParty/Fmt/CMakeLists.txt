# https://github.com/fmtlib/fmt/releases/tag/11.0.2
anemone_add_sdk(SdkFmt ${ANEMONE_MODULE_KIND})

target_compile_definitions(SdkFmt
    PUBLIC
        "FMT_EXCEPTIONS=0"
    PRIVATE
        "FMT_LIB_EXPORT"
)

add_subdirectory("source")
