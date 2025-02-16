#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

namespace Anemone
{
    using ThreadId = pid_t;
    using ThreadHandle = Interop::UnixPthreadThreadHandle;
}
