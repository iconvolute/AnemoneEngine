#pragma once
#include "AnemoneInterop/Headers.hxx"

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
        APPLICATION_API static void Initialize();
        APPLICATION_API static void Finalize();
        APPLICATION_API static HostSplashScreen& Get();

    public:
        virtual void SetText(std::string_view text) = 0;
        virtual void SetProgress(float progress) = 0;
    };
}
