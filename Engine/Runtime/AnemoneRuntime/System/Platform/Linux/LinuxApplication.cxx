#include "AnemoneRuntime/System/Platform/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/System/Application.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    UninitializedObject<Linux::ApplicationStatics> GApplicationStatics;
}

namespace Anemone
{
    void Application::ProcessMessages()
    {
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        (void)type;
        return {};
    }
}
