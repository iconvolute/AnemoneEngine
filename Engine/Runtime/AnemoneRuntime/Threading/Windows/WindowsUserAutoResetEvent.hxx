#pragma once
#include "AnemoneRuntime/Threading/Base/BaseUserAutoResetEvent.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"

namespace Anemone
{
    using UserAutoResetEvent = BaseUserAutoResetEvent<Internal::WindowsFutexTraits>;
}
