#include "AnemoneRuntime.HostApplication/Platform/Linux/LinuxHostSplashScreen.hxx"
#include "AnemoneRuntime.Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<LinuxHostSplashScreen> gLinuxHostSplashScreen{};
    }

    void HostSplashScreen::Initialize()
    {
        gLinuxHostSplashScreen.Create();
    }

    void HostSplashScreen::Finalize()
    {
        gLinuxHostSplashScreen.Destroy();
    }

    HostSplashScreen& HostSplashScreen::Get()
    {
        return *gLinuxHostSplashScreen;
    }

    LinuxHostSplashScreen::LinuxHostSplashScreen() = default;

    LinuxHostSplashScreen::~LinuxHostSplashScreen() = default;

    void LinuxHostSplashScreen::SetText(std::string_view text)
    {
        AE_PANIC("Not implemented");
        (void)text;
    }

    void LinuxHostSplashScreen::SetProgress(float progress)
    {
        AE_PANIC("Not implemented");
        (void)progress;
    }
}
