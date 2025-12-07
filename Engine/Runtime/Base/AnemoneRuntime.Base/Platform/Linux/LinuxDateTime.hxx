#pragma once
#include "AnemoneRuntime.Base/DateTime.hxx"

namespace Anemone
{
    class LinuxDateTime final
    {
    public:
        ANEMONE_RUNTIME_BASE_API static DateTime GetCurrentDateTime();
        ANEMONE_RUNTIME_BASE_API static DateTime GetCurrentDateTimeUtc();
        ANEMONE_RUNTIME_BASE_API static Duration GetCurrentTimeZoneBias();
    };
}
