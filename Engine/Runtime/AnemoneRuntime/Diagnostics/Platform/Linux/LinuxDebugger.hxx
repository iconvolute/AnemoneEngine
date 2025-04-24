#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"

#include <optional>

namespace Anemone::Internal
{
    struct LinuxDebuggerStatics final
    {
        LinuxDebuggerStatics();
        ~LinuxDebuggerStatics();
    };

    extern UninitializedObject<LinuxDebuggerStatics> GDebuggerStatics;
}
