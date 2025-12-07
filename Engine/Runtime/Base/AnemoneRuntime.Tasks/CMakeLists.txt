target_sources(AnemoneRuntime.Base
    PRIVATE
        "DefaultTaskScheduler.cxx"
        "DefaultTaskWorker.cxx"
        "Parallel.cxx"
        "Task.cxx"
        "TaskAwaiter.cxx"
        "TaskQueue.cxx"
        "TaskScheduler.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "DefaultTaskScheduler.hxx"
        "DefaultTaskWorker.hxx"
        "Parallel.hxx"
        "Task.hxx"
        "TaskAwaiter.hxx"
        "TaskQueue.hxx"
        "TaskScheduler.hxx"
)
