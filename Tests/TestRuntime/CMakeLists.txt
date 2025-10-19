anemone_add_test(TestRuntime)

target_link_libraries(TestRuntime
    PRIVATE
        AnemoneRuntime
        AnemoneTasks
        AnemoneNetwork
        AnemoneEntryPoint
)

add_subdirectory("Source")
add_subdirectory("Resources")
