target_sources(AnemoneRuntime
    PRIVATE
        "Awaiter.hxx"
        "Parallel.hxx"
        "Task.hxx"
        "TaskScheduler.hxx"
)

#add_subdirectory("Platform")
add_subdirectory("Private")
