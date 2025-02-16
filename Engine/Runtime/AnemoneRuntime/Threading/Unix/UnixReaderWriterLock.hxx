#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

namespace Anemone::Internal
{
    using PlatformReaderWriterLock = pthread_rwlock_t;
}
