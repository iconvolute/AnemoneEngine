#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsPlatform.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidPlatform.hxx"
#else
#error "Unsupported platform"
#endif

namespace Anemone
{
    struct Platform final
    {
        static RUNTIME_API void Initialize();
        static RUNTIME_API void Finalize();
    };
}
