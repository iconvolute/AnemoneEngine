if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsConditionVariable.cxx"
        "WindowsCriticalSection.cxx"
        "WindowsCurrentThread.cxx"
        "WindowsReaderWriterLock.cxx"
        "WindowsSemaphore.cxx"
        "WindowsThread.cxx"
        "WindowsThreading.hxx"
        "WindowsMonitor.cxx"
)

endif()
