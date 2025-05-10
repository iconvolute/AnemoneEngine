#include "AnemoneRuntime/App/IApplication.hxx"
#include "AnemoneRuntime/App/Platform/Linux/Application.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<Application> GApplication{};
    }

    void IApplication::Initialize(IApplicationEvents* events)
    {
        GApplication.Create(events);
    }

    void IApplication::Finalize()
    {
        GApplication.Destroy();
    }

    IApplication& IApplication::Get()
    {
        return *GApplication;
    }
}

namespace Anemone
{
    Application::Application(IApplicationEvents* events)
        : _events{events}
    {
    }

    void Application::ProcessMessages()
    {
    }

    std::unique_ptr<IWindow> Application::CreateWindow(WindowType type, WindowMode mode)
    {
        (void)type;
        (void)mode;
        return nullptr;
    }
}
