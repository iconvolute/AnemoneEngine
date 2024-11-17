#include "AnemoneRuntime/App/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/App/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/App/Windows/WindowsRuntime.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone::App
{
    Reference<Window> Application::MakeWindow(WindowType type)
    {
        return new WindowsWindow(this, type);
    }

    void Application::ProcessMessages()
    {
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        GWindowsInput->Poll();
    }
};
