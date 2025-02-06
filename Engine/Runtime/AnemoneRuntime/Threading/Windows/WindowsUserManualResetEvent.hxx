#pragma once
#include "AnemoneRuntime/Threading/Base/BaseUserManualResetEvent.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"

namespace Anemone
{
    using UserManualResetEvent = BaseUserManualResetEvent<Internal::WindowsFutexTraits>;
}
