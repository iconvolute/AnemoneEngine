#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Diagnostic
{
    enum class TraceLevel
    {
        Verbose,
        Info,
        Warning,
        Error,
        Critical,
        None,
    };
}
