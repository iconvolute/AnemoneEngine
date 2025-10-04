#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneApplication/HostWindow.hxx"

namespace Anemone
{
    class HostApplicationEvents;

    class HostApplication
    {
    public:
        HostApplication() = default;
        HostApplication(HostApplication const&) = delete;
        HostApplication(HostApplication&&) = delete;
        virtual ~HostApplication() = default;
        HostApplication& operator=(HostApplication const&) = delete;
        HostApplication& operator=(HostApplication&&) = delete;

    public:
        APPLICATION_API static void Initialize(HostApplicationEvents& events);
        APPLICATION_API static void Finalize();
        APPLICATION_API static HostApplication& Get();

    public:
        virtual void ProcessMessages() = 0;

        virtual Reference<HostWindow> MakeWindow(WindowType windowType, WindowMode windowMode) = 0;
    };
}
