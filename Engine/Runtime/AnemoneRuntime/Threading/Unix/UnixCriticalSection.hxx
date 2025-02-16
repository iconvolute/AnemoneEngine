#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

namespace Anemone::Internal
{
    using PlatformCriticalSection = pthread_mutex_t;
}
