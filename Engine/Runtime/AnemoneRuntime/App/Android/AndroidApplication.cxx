#include "AnemoneRuntime/App/Android/AndroidApplication.hxx"
#include "AnemoneRuntime/App/Android/AndroidWindow.hxx"
#include "AnemoneRuntime/App/Android/AndroidRuntime.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::App
{
    Reference<Window> Application::MakeWindow(WindowType type)
    {
        return new AndroidWindow(this, type);
    }

    void Application::ProcessMessages()
    {
        // TODO: Implement
        // For now we close any window open

        this->m_windows.ForEach([this](Window& window)
        {
            WindowCloseEventArgs e{};
            this->OnWindowClose(window, e);
        });
    }
};
