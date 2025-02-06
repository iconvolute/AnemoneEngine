#pragma once
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"
#include "AnemoneRuntime/Threading/Base/BaseUserCriticalSection.hxx"

namespace Anemone
{
    using UserCriticalSection = BaseUserCriticalSection<Internal::WindowsFutexTraits>;
}
