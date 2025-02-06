#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX
#include "AnemoneRuntime/Threading/Base/BaseUserManualResetEvent.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"

namespace Anemone
{
    using UserManualResetEvent = BaseUserManualResetEvent<Internal::LinuxFutexTraits>;
}

#else
#include "AnemoneRuntime/Threading/ManualResetEvent.hxx"

namespace Anemone
{
    using UserManualResetEvent = BaseManualResetEvent;
}

#endif
