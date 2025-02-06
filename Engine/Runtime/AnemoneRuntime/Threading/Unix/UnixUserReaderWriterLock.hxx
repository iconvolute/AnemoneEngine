#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX
#include "AnemoneRuntime/Threading/Unix/UnixFutex.hxx"
#include "AnemoneRuntime/Threading/Base/BaseUserReaderWriterLock.hxx"

namespace Anemone
{
    using UserReaderWriterLock = BaseUserReaderWriterLock<Internal::LinuxFutexTraits>;
}

#else
#include "AnemoneRuntime/Threading/Unix/UnixReaderWriterLock.hxx"

namespace Anemone
{
    using UserReaderWriterLock = UnixReaderWriterLock;
}

#endif
