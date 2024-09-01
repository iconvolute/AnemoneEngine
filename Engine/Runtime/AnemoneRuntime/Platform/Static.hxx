#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Platform
{
    struct GenericPlatformStatic
    {
        static void Initialize();
        static void Finalize();
    };
}

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/Static.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Posix/Static.hxx"
#else
#error "Unsupported platform"
#endif
