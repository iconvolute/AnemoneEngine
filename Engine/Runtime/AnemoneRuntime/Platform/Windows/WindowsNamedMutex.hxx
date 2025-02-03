#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Internal
{
    struct NativeNamedMutexTraits final
    {
        static HANDLE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HANDLE value)
        {
            return value != nullptr;
        }

        static void Close(HANDLE value)
        {
            if (not CloseHandle(value))
            {
                AE_TRACE(Debug, "Failed to close named mutex: handle={}, error={}", fmt::ptr(value), GetLastError());
            }
        }
    };

    using NativeNamedMutex = Interop::base_SafeHandle<HANDLE, NativeNamedMutexTraits>;
}
