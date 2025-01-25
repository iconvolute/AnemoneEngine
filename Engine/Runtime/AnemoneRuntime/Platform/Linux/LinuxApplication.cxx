#include "AnemoneRuntime/Platform/Linux/LinuxApplication.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxDebugger.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseApplication.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxWindow.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Internal
{
    UninitializedObject<LinuxApplicationStatics> GLinuxApplicationStatics;
}

namespace Anemone
{
    void LinuxApplication::Initialize()
    {
        Internal::GLinuxApplicationStatics.Create();
    }

    void LinuxApplication::Finalize()
    {
        Internal::GLinuxApplicationStatics.Destroy();
    }

    void LinuxApplication::ProcessMessages()
    {
    }

    void LinuxApplication::SetEvents(IApplicationEvents* events)
    {
        Internal::GLinuxApplicationStatics->Events = events;
    }

    std::unique_ptr<Window> LinuxApplication::MakeWindow(WindowType type)
    {
        (void)type;
        return {};
    }
}
