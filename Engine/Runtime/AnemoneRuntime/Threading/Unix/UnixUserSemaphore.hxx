#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Threading/Base/BaseUserSemaphore.hxx"
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"

namespace Anemone
{
    using UserSemaphore = BaseUserSemaphore<Internal::LinuxFutexTraits>;
}

#else

#include "AnemoneRuntime/Threading/Unix/UnixSemaphore.hxx"

namespace Anemone
{
    using UserSemaphore = UnixSemaphore;
}

#endif
