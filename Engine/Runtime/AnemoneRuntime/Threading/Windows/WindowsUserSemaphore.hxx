#pragma once
#include "AnemoneRuntime/Threading/Base/BaseUserSemaphore.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"

namespace Anemone
{
    using UserSemaphore = BaseUserSemaphore<Internal::WindowsFutexTraits>;
}
