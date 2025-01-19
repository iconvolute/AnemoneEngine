#include "AnemoneRuntime/Platform/Linux/LinuxSplashScreen.hxx"

namespace Anemone
{
    void SplashScreen::Show()
    {
    }

    void SplashScreen::Hide()
    {
    }

    bool SplashScreen::IsVisible()
    {
        return false;
    }

    void SplashScreen::SetText(std::string_view text)
    {
        (void)text;
    }

    void SplashScreen::SetProgress(float progress)
    {
        (void)progress;
    }
}
