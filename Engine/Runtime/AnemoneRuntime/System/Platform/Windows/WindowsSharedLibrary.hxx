#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformSharedLibrary = Interop::Win32SafeSharedLibraryHandle;
}
