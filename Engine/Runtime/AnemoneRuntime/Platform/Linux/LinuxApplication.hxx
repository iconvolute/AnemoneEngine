#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Application.hxx"
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
