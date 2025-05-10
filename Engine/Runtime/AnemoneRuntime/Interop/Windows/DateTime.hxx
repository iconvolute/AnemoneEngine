#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"
#include "AnemoneRuntime/Base/DateTime.hxx"

namespace Anemone::Interop::Windows
{
    static constexpr int64_t DateAdjustOffset = 504911231999999999;
    inline constexpr int64_t TicksPerSecond = 1'000'000'000 / 100;

    constexpr Duration ToDuration(FILETIME const value, int64_t offset) noexcept
    {
        int64_t const ticks = std::bit_cast<int64_t>(value) + offset;

        return Duration{
            .Seconds = (ticks / TicksPerSecond),
            .Nanoseconds = (ticks % TicksPerSecond) * 100,
        };
    }

    constexpr Duration ToDuration(FILETIME const value) noexcept
    {
        int64_t const ticks = std::bit_cast<int64_t>(value);

        return Duration{
            .Seconds = (ticks / TicksPerSecond),
            .Nanoseconds = (ticks % TicksPerSecond) * 100,
        };
    }

    constexpr FILETIME ToFILETIME(Duration const value) noexcept
    {
        int64_t const ticks = (value.Seconds * TicksPerSecond) + (value.Nanoseconds / 100);

        return std::bit_cast<FILETIME>(ticks - DateAdjustOffset);
    }

    constexpr DateTime ToDateTime(FILETIME const& value)
    {
        return DateTime{
            .Inner = ToDuration(value, DateAdjustOffset)};
    }

    constexpr FILETIME ToFILETIME(DateTime const& value)
    {
        return ToFILETIME(value.Inner);
    }
    
    inline HRESULT LocalSystemTimeToFileTime(const SYSTEMTIME& localTime, FILETIME& fileTime)
    {
        SYSTEMTIME systemTime;

        if (TzSpecificLocalTimeToSystemTime(nullptr, &localTime, &systemTime))
        {
            if (SystemTimeToFileTime(&systemTime, &fileTime))
            {
                return S_OK;
            }
        }

        return HRESULT_FROM_WIN32(GetLastError());
    }

    inline HRESULT GetLocalTimeAsFileTime(FILETIME& fileTime)
    {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);

        if (SystemTimeToFileTime(&systemTime, &fileTime))
        {
            return S_OK;
        }

        return HRESULT_FROM_WIN32(GetLastError());
    }
}
