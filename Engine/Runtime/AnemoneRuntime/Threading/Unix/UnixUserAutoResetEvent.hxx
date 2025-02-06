#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX
#include "AnemoneRuntime/Threading/Base/BaseUserAutoResetEvent.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"

namespace Anemone
{
    using UserAutoResetEvent = BaseUserAutoResetEvent<Internal::LinuxFutexTraits>;
}

#else
#include "AnemoneRuntime/Threading/AutoResetEvent.hxx"

namespace Anemone
{
    using UserAutoResetEvent = BaseAutoResetEvent;
}

#endif
