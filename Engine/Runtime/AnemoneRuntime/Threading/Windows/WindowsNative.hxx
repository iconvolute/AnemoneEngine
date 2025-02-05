#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

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

namespace Anemone::Internal
{
    using NativeThreadHandle = Interop::Win32SafeHandle;

    using NativeThreadId = DWORD;
}
