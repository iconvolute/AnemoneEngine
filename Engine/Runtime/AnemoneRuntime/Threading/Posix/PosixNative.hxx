#pragma once
#include "AnemoneRuntime/Platform/Unix/Types.hxx"
#include "AnemoneRuntime/Platform/Base/BaseSafeHandle.hxx"

namespace Anemone::Internal
{
    struct NativeThreadTraits final
    {
        static pthread_t Invalid()
        {
            return {};
        }

        static bool IsValid(pthread_t value)
        {
            return value != Invalid();
        }

        static bool Reset(pthread_t value)
        {
            (void)value;
            // No-op?
            return true;
        }
    };

    using NativeThreadHandle = Interop::base_SafeHandle<pthread_t, NativeThreadTraits>;

    struct NativeThreadIdTraits final
    {
        static pid_t Invalid()
        {
            return -1;
        }

        static bool IsValid(pid_t value)
        {
            return value > 0;
        }

        static bool Reset(pid_t value)
        {
            (void)value;
            // No-op?
            return true;
        }
    };

    using NativeThreadId = pid_t;
}
