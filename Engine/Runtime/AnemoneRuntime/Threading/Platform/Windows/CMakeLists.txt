if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsConditionVariable.cxx"
        "WindowsCriticalSection.cxx"
        "WindowsCurrentThread.cxx"
        "WindowsReaderWriterLock.cxx"
        "WindowsSemaphore.cxx"
        "WindowsThread.cxx"
        "WindowsMonitor.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsThreading.hxx"
)

endif()
