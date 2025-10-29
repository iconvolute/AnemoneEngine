if(ANDROID OR LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "UnixConditionVariable.cxx"
        "UnixCriticalSection.cxx"
        "UnixCurrentThread.cxx"
        "UnixMonitor.cxx"
        "UnixReaderWriterLock.cxx"
        "UnixSemaphore.cxx"
        "UnixThread.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "UnixThreading.hxx"
)

endif()
