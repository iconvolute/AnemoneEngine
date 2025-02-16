#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone
{
    using ThreadId = DWORD;
    using ThreadHandle = Interop::Win32SafeHandle;
}
