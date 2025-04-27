#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

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
}
