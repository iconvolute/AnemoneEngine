#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX
#include "AnemoneRuntime/Threading/Base/BaseUserCriticalSection.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"

namespace Anemone
{
    using UserCriticalSection = BaseUserCriticalSection<Internal::LinuxFutexTraits>;
}

#else
#include "AnemoneRuntime/Threading/Unix/UnixCriticalSection.hxx"

namespace Anemone
{
    using UserCriticalSection = CriticalSection;
}

#endif
