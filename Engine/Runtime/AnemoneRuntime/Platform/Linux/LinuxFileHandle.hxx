#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <unistd.h>

namespace Anemone::Internal
{
    struct NativeFileHandleTraits final
    {
        static int Invalid()
        {
            return -1;
        }

        static bool IsValid(int value)
        {
            return value >= 0;
        }

        static void Close(int value)
        {
            if (close(value) != 0)
            {
                AE_TRACE(Debug, "Failed to close file handle: handle={}, error={}", value, errno);
            }
        }
    };

    using NativeFileHandle = Interop::base_SafeHandle<int, NativeFileHandleTraits>;
}
