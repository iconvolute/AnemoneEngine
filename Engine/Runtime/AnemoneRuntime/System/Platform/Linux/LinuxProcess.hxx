#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformProcess = Interop::UnixSafePidHandle;
}
