#pragma once

namespace Anemone::Diagnostics
{
    enum class TraceLevel : uint8_t
    {
        Verbose,
        Debug,
        Information,
        Warning,
        Error,
        Fatal,
        None,
    };
}
