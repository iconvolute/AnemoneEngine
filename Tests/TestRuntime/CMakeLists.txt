anemone_add_test(TestRuntime)

target_link_libraries(TestRuntime
    PRIVATE
        AnemoneRuntime
        AnemoneTasks
        AnemoneNetwork
)

add_subdirectory("Source")
add_subdirectory("Resources")
