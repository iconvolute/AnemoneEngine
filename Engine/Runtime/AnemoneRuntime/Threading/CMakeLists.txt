target_sources(AnemoneRuntime
    PRIVATE
        "AutoResetEvent.cxx"
        "AutoResetEvent.hxx"

        "CancellationToken.cxx"
        "CancellationToken.hxx"

        "ConcurrentAccess.cxx"
        "ConcurrentAccess.hxx"

        "ConditionVariable.cxx"
        "ConditionVariable.hxx"

        "CriticalSection.cxx"
        "CriticalSection.hxx"

        "CriticalSectionPool.cxx"
        "CriticalSectionPool.hxx"

        "CurrentThread.cxx"
        "CurrentThread.hxx"

        "Lock.cxx"
        "Lock.hxx"

        "ManualResetEvent.cxx"
        "ManualResetEvent.hxx"

        "MpmcQueue.cxx"
        "MpmcQueue.hxx"

        "MpscQueue.cxx"
        "MpscQueue.hxx"

        "ReaderWriterLock.cxx"
        "ReaderWriterLock.hxx"

        "Runnable.cxx"
        "Runnable.hxx"

        "Semaphore.cxx"
        "Semaphore.hxx"

        "SequenceIndex.cxx"
        "SequenceIndex.hxx"

        "SequentialLock.cxx"
        "SequentialLock.hxx"

        "Spinlock.cxx"
        "Spinlock.hxx"

        "SpinWait.cxx"
        "SpinWait.hxx"

        "SpscQueue.cxx"
        "SpscQueue.hxx"

        "Thread.cxx"
        "Thread.hxx"

        "UserAutoResetEvent.cxx"
        "UserAutoResetEvent.hxx"

        "UserCriticalSection.cxx"
        "UserCriticalSection.hxx"

        "UserManualResetEvent.cxx"
        "UserManualResetEvent.hxx"

        "UserReaderWriterLock.cxx"
        "UserReaderWriterLock.hxx"

        "UserSemaphore.cxx"
        "UserSemaphore.hxx"
)

add_subdirectory("Base")
if (WIN32)
add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
add_subdirectory("Unix")
endif()
