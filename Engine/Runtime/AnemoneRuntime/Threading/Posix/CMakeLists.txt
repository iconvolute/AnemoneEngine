target_sources(AnemoneRuntime
    PRIVATE
        "PosixNative.hxx"

        "PosixConditionVariable.cxx"
        "PosixCriticalSection.cxx"
        "PosixReaderWriterLock.cxx"
        "PosixSemaphore.cxx"
        "PosixThread.cxx"
        "PosixUserAutoResetEvent.cxx"
        "PosixUserCriticalSection.cxx"
        "PosixUserManualResetEvent.cxx"
        "PosixUserSemaphore.cxx"
        "PosixYielding.cxx"
)
