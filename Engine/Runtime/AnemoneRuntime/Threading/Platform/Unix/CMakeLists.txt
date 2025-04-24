if(ANDROID OR LINUX)

target_sources(AnemoneRuntime
    PRIVATE
        "UnixConditionVariable.cxx"
        "UnixCriticalSection.cxx"
        "UnixCurrentThread.cxx"
        "UnixReaderWriterLock.cxx"
        "UnixSemaphore.cxx"
        "UnixThread.cxx"
        "UnixThreading.hxx"
)

endif()
