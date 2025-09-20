target_sources(AnemoneRuntime
    PRIVATE
        "Awaiter.cxx"
        "Parallel.cxx"
        "Task.cxx"
        "TaskQueue.cxx"
        "TaskScheduler.cxx"
        "TaskWorker.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "TaskQueue.hxx"
        "TaskScheduler.hxx"
        "TaskWorker.hxx"
)
