#pragma once
#include "AnemoneRuntime.Base/DateTime.hxx"

namespace Anemone
{
    struct WindowsDateTime final
    {
        ANEMONE_RUNTIME_BASE_API static DateTime GetCurrentDateTime();
        ANEMONE_RUNTIME_BASE_API static DateTime GetCurrentDateTimeUtc();
        ANEMONE_RUNTIME_BASE_API static Duration GetCurrentTimeZoneBias();
    };
}
