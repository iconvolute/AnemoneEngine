anemone_add_module(AnemoneTasks
    ALIAS   Anemone::Tasks
    EXPORT  "TASKS_API"
)

target_link_libraries(AnemoneTasks
    PUBLIC
        AnemoneRuntime
)

add_subdirectory("AnemoneTasks")
