#pragma once
#include "AnemoneApplication/HostSplashScreen.hxx"

namespace Anemone
{
    class LinuxHostSplashScreen final
        : public HostSplashScreen
    {
    public:
        LinuxHostSplashScreen();
        LinuxHostSplashScreen(LinuxHostSplashScreen const&) = delete;
        LinuxHostSplashScreen(LinuxHostSplashScreen&&) = delete;
        ~LinuxHostSplashScreen() override;
        LinuxHostSplashScreen& operator=(LinuxHostSplashScreen const&) = delete;
        LinuxHostSplashScreen& operator=(LinuxHostSplashScreen&&) = delete;

    public:
        void SetText(std::string_view text) override;
        void SetProgress(float progress) override;
    };
}
