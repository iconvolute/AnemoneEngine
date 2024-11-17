#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if !ANEMONE_FEATURE_FUTEX
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    using UserCriticalSection = CriticalSection;
}

#elif ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsUserCriticalSection.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixUserCriticalSection.hxx"
#else
#error "Not implemented"
#endif
