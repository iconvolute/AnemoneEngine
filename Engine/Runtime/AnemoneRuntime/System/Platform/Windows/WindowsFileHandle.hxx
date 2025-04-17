#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformFileHandle = Interop::Win32SafeFileHandle;
}
