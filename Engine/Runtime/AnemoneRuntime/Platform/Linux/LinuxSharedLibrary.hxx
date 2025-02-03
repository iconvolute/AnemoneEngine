#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <dlfcn.h>

namespace Anemone::Internal
{
    struct NativeSharedLibraryTraits final
    {
        static void* Invalid()
        {
            return nullptr;
        }

        static bool IsValid(void* value)
        {
            return value != nullptr;
        }

        static void Close(void* value)
        {
            if (dlclose(value))
            {
                AE_TRACE(Debug, "Failed to close shared library: handle={}, error={}", fmt::ptr(value), dlerror());
            }
        }
    };

    using NativeSharedLibrary = Interop::base_SafeHandle<void*, NativeSharedLibraryTraits>;
}
