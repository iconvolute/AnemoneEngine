anemone_add_target(
    TEST
    NAME
        TestNumerics
    BUILD_DEPENDENCIES
        PRIVATE
            AnemoneRuntime.Base
            AnemoneRuntime.EntryPoint
)

add_subdirectory("Source")
add_subdirectory("Resources")
