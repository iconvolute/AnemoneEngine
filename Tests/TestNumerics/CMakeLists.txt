anemone_add_test(TestNumerics)

target_link_libraries(TestNumerics
    PRIVATE
        AnemoneRuntime
)

add_subdirectory("Source")
add_subdirectory("Resources")
