#include "AnemoneRuntime.Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime.Base/Platform/Windows/WindowsDateTime.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

namespace Anemone
{
    DateTime WindowsDateTime::GetCurrentDateTime()
    {
        FILETIME ft;
        Interop::Windows::GetLocalTimeAsFileTime(ft);
        return Interop::Windows::ToDateTime(ft);
    }

    DateTime WindowsDateTime::GetCurrentDateTimeUtc()
    {
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        return Interop::Windows::ToDateTime(ft);
    }

    Duration WindowsDateTime::GetCurrentTimeZoneBias()
    {
        DYNAMIC_TIME_ZONE_INFORMATION dtzi;

        int64_t seconds = 0;

        switch (GetDynamicTimeZoneInformation(&dtzi))
        {
        case TIME_ZONE_ID_INVALID:
            AE_PANIC("Cannot obtain timezone ID: {}", GetLastError());
            break;

        default:
        case TIME_ZONE_ID_UNKNOWN:
            break;

        case TIME_ZONE_ID_STANDARD:
            seconds += static_cast<int64_t>(dtzi.StandardBias) * 60;
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            seconds += static_cast<int64_t>(dtzi.DaylightBias) * 60;
            break;
        }

        seconds += static_cast<int64_t>(dtzi.Bias) * 60;

        return Duration{
            .Seconds = seconds,
            .Nanoseconds = 0,
        };
    }
}

namespace Anemone
{
    DateTime DateTime::Now()
    {
        return WindowsDateTime::GetCurrentDateTime();
    }

    DateTime DateTime::UtcNow()
    {
        return WindowsDateTime::GetCurrentDateTimeUtc();
    }

    DateTimeOffset DateTimeOffset::Now()
    {
        return DateTimeOffset{
            .Local = WindowsDateTime::GetCurrentDateTime(),
            .Bias = WindowsDateTime::GetCurrentTimeZoneBias(),
        };
    }

    Duration DateTimeOffset::GetCurrentTimeZoneBias()
    {
        return WindowsDateTime::GetCurrentTimeZoneBias();
    }
}
