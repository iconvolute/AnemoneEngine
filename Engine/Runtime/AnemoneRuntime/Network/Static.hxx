#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Network
{
    struct GenericNetworkStatic
    {
        static void Initialize();
        static void Finalize();
    };
}

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Network/Windows/Static.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Network/Posix/Static.hxx"
#else
#error "Unsupported platform"
#endif
