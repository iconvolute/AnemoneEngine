#include "AnemoneRuntime/Platform/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone
{
    static UninitializedObject<LinuxApplicationStatics> GLinuxApplicationStatics;
}

namespace Anemone
{
    void Application::Initialize()
    {
        GLinuxApplicationStatics.Create();
    }

    void Application::Finalize()
    {
        GLinuxApplicationStatics.Destroy();
    }

    void Application::ProcessMessages()
    {
    }

    std::unique_ptr<Window> Application::MakeWindow(WindowType type)
    {
        (void)type;
        return {};
    }
}
