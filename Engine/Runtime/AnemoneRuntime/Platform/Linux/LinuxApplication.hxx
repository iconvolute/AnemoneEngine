#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseApplication.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxWindow.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Internal
{
    struct LinuxApplicationStatics final
    {
        IApplicationEvents* Events = nullptr;

        IntrusiveList<Window> LinuxCollection{};
    };

    extern UninitializedObject<LinuxApplicationStatics> GLinuxApplicationStatics;
}

namespace Anemone
{
    struct LinuxApplication final
    {
        static RUNTIME_API void Initialize();
        static RUNTIME_API void Finalize();
        static RUNTIME_API void ProcessMessages();
        static RUNTIME_API void SetEvents(IApplicationEvents* events);

        static RUNTIME_API std::unique_ptr<Window> MakeWindow(WindowType type);
    };

    using Application = LinuxApplication;
}
