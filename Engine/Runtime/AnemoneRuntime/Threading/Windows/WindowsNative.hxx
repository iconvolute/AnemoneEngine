#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone::Detail
{
    struct NativeConditionVariable final
    {
        CONDITION_VARIABLE Inner;
    };

    struct NativeCriticalSection final
    {
        CRITICAL_SECTION Inner;
    };

    struct NativeReaderWriterLock final
    {
        SRWLOCK Inner;
    };

    struct NativeSemaphore final
    {
        HANDLE Inner;
    };

    struct NativeThread final
    {
        HANDLE Handle;
        DWORD Id;
    };
}
