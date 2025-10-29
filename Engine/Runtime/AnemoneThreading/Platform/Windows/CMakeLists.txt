if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsConditionVariable.cxx"
        "WindowsCriticalSection.cxx"
        "WindowsCurrentThread.cxx"
        "WindowsMonitor.cxx"
        "WindowsReaderWriterLock.cxx"
        "WindowsSemaphore.cxx"
        "WindowsThread.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsThread.hxx"
        "WindowsThreading.hxx"
)

endif()
