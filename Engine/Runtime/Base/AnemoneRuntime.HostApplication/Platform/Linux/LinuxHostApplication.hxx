#pragma once
#include "AnemoneRuntime.HostApplication/HostApplication.hxx"
#include "AnemoneRuntime.HostApplication/Platform/Linux/LinuxHostWindow.hxx"
#include "AnemoneRuntime.Base/Intrusive.hxx"

namespace Anemone
{
    class LinuxHostWindow;

    class LinuxHostApplication final
        : public HostApplication
    {
    public:
        LinuxHostApplication(HostApplicationEvents& e);

        LinuxHostApplication(LinuxHostApplication const&) = delete;

        LinuxHostApplication(LinuxHostApplication&&) = delete;

        virtual ~LinuxHostApplication();

        LinuxHostApplication& operator=(LinuxHostApplication const&) = delete;

        LinuxHostApplication& operator=(LinuxHostApplication&&) = delete;

    public:
        IntrusiveList<LinuxHostWindow> windows{};
        HostApplicationEvents* events{};

        void ProcessMessages() override;

        Reference<HostWindow> MakeWindow(WindowType windowType, WindowMode windowMode) override;
    };
}
