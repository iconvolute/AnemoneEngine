#pragma once
#include "AnemoneBase/DateTime.hxx"

namespace Anemone
{
    struct WindowsDateTime final
    {
        RUNTIME_API static DateTime GetCurrentDateTime();
        RUNTIME_API static DateTime GetCurrentDateTimeUtc();
        RUNTIME_API static Duration GetCurrentTimeZoneBias();
    };
}
