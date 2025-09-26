#pragma once
#include "AnemoneRuntime/Base/DateTime.hxx"

namespace Anemone
{
    class LinuxDateTime final
    {
    public:
        RUNTIME_API static DateTime GetCurrentDateTime();
        RUNTIME_API static DateTime GetCurrentDateTimeUtc();
        RUNTIME_API static Duration GetCurrentTimeZoneBias();
    };
}
