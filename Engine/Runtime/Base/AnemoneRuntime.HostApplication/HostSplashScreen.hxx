#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <string_view>

namespace Anemone
{
    class HostSplashScreen
    {
    public:
        HostSplashScreen() = default;
        HostSplashScreen(HostSplashScreen const&) = delete;
        HostSplashScreen(HostSplashScreen&&) = delete;
        virtual ~HostSplashScreen() = default;
        HostSplashScreen& operator=(HostSplashScreen const&) = delete;
        HostSplashScreen& operator=(HostSplashScreen&&) = delete;

    public:
        ANEMONE_RUNTIME_BASE_API static void Initialize();
        ANEMONE_RUNTIME_BASE_API static void Finalize();
        ANEMONE_RUNTIME_BASE_API static HostSplashScreen& Get();

    public:
        virtual void SetText(std::string_view text) = 0;
        virtual void SetProgress(float progress) = 0;
    };
}
