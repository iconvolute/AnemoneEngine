target_sources(AnemoneRuntime
    PRIVATE
        "WindowsConditionVariable.hxx"

        "WindowsCriticalSection.hxx"

        "WindowsFutex.hxx"

        "WindowsReaderWriterLock.hxx"

        "WindowsSemaphore.hxx"

        "WindowsThread.cxx"

        "WindowsUserAutoResetEvent.cxx"
        "WindowsUserCriticalSection.cxx"
        "WindowsUserManualResetEvent.cxx"
        "WindowsUserSemaphore.cxx"
        "WindowsYielding.cxx"
)
