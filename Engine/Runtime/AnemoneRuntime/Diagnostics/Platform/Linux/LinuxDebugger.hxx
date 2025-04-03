#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include "AnemoneRuntime/Diagnostics/Private/ConsoleTraceListener.hxx"

#include <optional>

namespace Anemone::Private
{
    struct LinuxDebuggerStatics final
    {
        LinuxDebuggerStatics();
        ~LinuxDebuggerStatics();
    };

    extern UninitializedObject<LinuxDebuggerStatics> GDebuggerStatics;
}
