#pragma once
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformSharedLibrary = Interop::Windows::SafeSharedLibraryHandle;
    using PlatformProcess = Interop::Windows::SafeHandle;
    using PlatformFileHandle = Interop::Windows::SafeFileHandle;
}
