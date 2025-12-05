anemone_add_target(
    MODULE
    NAME
        AnemoneTasks
    ALIAS
        Anemone::Tasks
    EXPORT
        "TASKS_API"
    BUILD_DEPENDENCIES
        PUBLIC
            AnemoneRuntime
            AnemoneProfiler
)

add_subdirectory("AnemoneTasks")
