anemone_add_target(
    TEST
    NAME
        TestRuntime
    BUILD_DEPENDENCIES
        PRIVATE
            AnemoneRuntime
            AnemoneTasks
            AnemoneNetwork
            AnemoneEntryPoint
)

add_subdirectory("Source")
add_subdirectory("Resources")
