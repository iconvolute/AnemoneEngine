#include "AnemoneRuntime/App/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/App/Linux/LinuxWindow.hxx"
#include "AnemoneRuntime/App/Linux/LinuxRuntime.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::App
{
    Reference<Window> Application::MakeWindow(WindowType type)
    {
        return new LinuxWindow(this, type);
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
