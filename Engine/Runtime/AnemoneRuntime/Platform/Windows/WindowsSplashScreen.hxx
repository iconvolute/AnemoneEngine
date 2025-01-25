#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>

namespace Anemone
{
    struct WindowsSplashScreen final
    {
        void Show();
        void Hide();
        bool IsVisible();
        void SetText(std::string_view text);
        void SetProgress(float progress);
    };

    using SplashScreen = WindowsSplashScreen;
}
