#include "AnemoneRuntime/App/Application.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

namespace Anemone::App
{
    Application* Application::GCurrent = nullptr;

    Application::Application()
    {
        AE_ENSURE(GCurrent == nullptr);
        GCurrent = this;
    }

    Application::~Application()
    {
        AE_ENSURE(GCurrent == this);
        GCurrent = nullptr;
    }


    void Application::AddWindow(Window& window)
    {
        this->m_windows.PushBack(&window);
    }

    void Application::RemoveWindow(Window& window)
    {
        this->m_windows.Remove(&window);
    }
}
