#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformSemaphore = Interop::Win32SafeHandle;
}
