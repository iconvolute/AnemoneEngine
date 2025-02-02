#include "AnemoneRuntime/Platform/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <memory>

namespace Anemone::Internal
{
    UninitializedObject<LinuxApplicationStatics> GLinuxApplicationStatics;
}

namespace Anemone
{
    void Application::Initialize()
    {
        Internal::GLinuxApplicationStatics.Create();
    }

    void Application::Finalize()
    {
        Internal::GLinuxApplicationStatics.Destroy();
    }

    void Application::ProcessMessages()
    {
    }

    void Application::SetEvents(IApplicationEvents* events)
    {
        Internal::GLinuxApplicationStatics->Events = events;
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        (void)type;
        return {};
    }
}
