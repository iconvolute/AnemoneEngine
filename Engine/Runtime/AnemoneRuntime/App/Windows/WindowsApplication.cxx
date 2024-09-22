#include "AnemoneRuntime/App/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/App/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/App/Windows/WindowsRuntime.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::App
{
    Reference<Window> Application::MakeWindow(WindowType type)
    {
        return new WindowsWindow(type);
    }

    void Application::ProcessMessages()
    {
        AE_ENSURE(GCurrent != nullptr);

        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        GWindowsInput->Poll();
    }
};
