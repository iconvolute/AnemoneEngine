#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Internal
{
    struct NativeFileHandleTraits final
    {
        static HANDLE Invalid()
        {
            return INVALID_HANDLE_VALUE;
        }

        static bool IsValid(HANDLE value)
        {
            return value != INVALID_HANDLE_VALUE;
        }

        static void Close(HANDLE value)
        {
            if (not CloseHandle(value))
            {
                AE_TRACE(Debug, "Failed to close file handle: handle={}, error={}", fmt::ptr(value), GetLastError());
            }
        }
    };

    using NativeFileHandle = Interop::base_SafeHandle<HANDLE, NativeFileHandleTraits>;
}
