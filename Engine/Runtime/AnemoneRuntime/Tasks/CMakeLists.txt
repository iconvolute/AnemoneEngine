target_sources(AnemoneRuntime
    PUBLIC FILE_SET HEADERS FILES
        "Awaiter.hxx"
        "Parallel.hxx"
        "Task.hxx"
        "TaskScheduler.hxx"
)

#add_subdirectory("Platform")
add_subdirectory("Internal")
