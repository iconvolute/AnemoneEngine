#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"

namespace Anemone::Internal
{
    struct NativeProcessHandleTraits final
    {
        static pid_t Invalid()
        {
            return -1;
        }

        static bool IsValid(pid_t value)
        {
            return value > 0;
        }

        static void Close(pid_t value)
        {
            (void)value;
            // No-op?
        }
    };

    using NativeProcessHandle = Interop::base_SafeHandle<pid_t, NativeProcessHandleTraits>;
}
