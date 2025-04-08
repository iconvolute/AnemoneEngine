#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/System/Application.hxx"
#include "AnemoneRuntime/System/Platform/Linux/LinuxWindow.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <memory>

namespace Anemone::Private
{
    struct LinuxApplicationStatics final
    {
        IntrusiveList<Window> LinuxCollection{};
    };

    extern UninitializedObject<LinuxApplicationStatics> GApplicationStatics;
}
