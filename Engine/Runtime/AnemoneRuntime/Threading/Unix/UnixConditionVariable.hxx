#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

namespace Anemone::Internal
{
    using PlatformConditionVariable = pthread_cond_t;
}
