#include "AnemoneRuntime/System/Platform/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    UninitializedObject<LinuxApplicationStatics> GApplicationStatics;
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
