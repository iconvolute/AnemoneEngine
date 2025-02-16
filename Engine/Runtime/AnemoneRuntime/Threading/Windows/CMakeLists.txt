target_sources(AnemoneRuntime
    PRIVATE
        "WindowsConditionVariable.cxx"
        "WindowsConditionVariable.hxx"

        "WindowsCriticalSection.cxx"
        "WindowsCriticalSection.hxx"
        
        "WindowsCurrentThread.cxx"

        "WindowsFutex.hxx"

        "WindowsReaderWriterLock.cxx"
        "WindowsReaderWriterLock.hxx"

        "WindowsSemaphore.cxx"
        "WindowsSemaphore.hxx"

        "WindowsThread.cxx"
        "WindowsThread.hxx"
)
