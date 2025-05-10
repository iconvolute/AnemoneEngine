#pragma once
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformSharedLibrary = Interop::Linux::SafeSharedLibraryHandle;
    using PlatformProcess = Interop::Linux::SafePidHandle;
    using PlatformFileHandle = Interop::Linux::SafeFdHandle;
}
