#pragma once
#include "AnemoneRuntime/Threading/Base/BaseUserReaderWriterLock.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsFutex.hxx"

namespace Anemone
{
    using UserReaderWriterLock = BaseUserReaderWriterLock<Internal::WindowsFutexTraits>;
}
