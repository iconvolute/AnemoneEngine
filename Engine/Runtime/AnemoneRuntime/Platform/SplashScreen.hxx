#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>

namespace Anemone
{
    // TODO: Rewrite as proper singleton?
    // Splash screen should be shown only at startup (so it isn't a singleton - it won't live long enough to be one)
    // However, it should be available globally? (not sure about this - only process initialization should use it)
    // It is stateful and only one instance should be shown at a time (so it is a singleton)
    struct SplashScreen
    {
        RUNTIME_API static void Show();
        RUNTIME_API static void Hide();
        RUNTIME_API static bool IsVisible();

        RUNTIME_API static void SetText(std::string_view text);
        RUNTIME_API static void SetProgress(float progress);
    };
}
