target_sources(AnemoneRuntime
    PRIVATE
        "UnixConditionVariable.cxx"
        "UnixConditionVariable.hxx"

        "UnixCriticalSection.cxx"
        "UnixCriticalSection.hxx"

        "UnixCurrentThread.cxx"

        "UnixFutex.hxx"

        "UnixReaderWriterLock.cxx"
        "UnixReaderWriterLock.hxx"

        "UnixSemaphore.cxx"
        "UnixSemaphore.hxx"

        "UnixThread.cxx"
        "UnixThread.hxx"
)
