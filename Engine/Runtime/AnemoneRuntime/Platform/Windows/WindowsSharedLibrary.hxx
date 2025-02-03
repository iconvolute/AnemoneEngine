#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Internal
{
    struct NativeSharedLibraryTraits final
    {
        static HMODULE Invalid()
        {
            return nullptr;
        }

        static bool IsValid(HMODULE value)
        {
            return value != nullptr;
        }

        static void Close(HMODULE value)
        {
            if (not FreeLibrary(value))
            {
                AE_TRACE(Debug, "Failed to close shared library: handle={}, error={}", fmt::ptr(value), GetLastError());
            }
        }
    };

    using NativeSharedLibrary = Interop::base_SafeHandle<HMODULE, NativeSharedLibraryTraits>;
}
