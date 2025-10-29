#pragma once
#include "AnemoneInterop/Windows/Headers.hxx"

namespace Anemone::Interop::Windows
{
    namespace Implementation
    {
        template <typename Callback = bool(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor)>
        BOOL CALLBACK EnumDisplayMonitorsWorker(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
        {
            Callback* const callback = std::bit_cast<Callback*>(dwData);
            return (*callback)(hMonitor, hdcMonitor, lprcMonitor);
        }
    }

    template <typename Callback = bool(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor)>
    [[nodiscard]] HRESULT EnumDisplayMonitors(HDC hdc, LPCRECT lprcClip, Callback&& callback)
    {
        Callback localCallback = std::forward<Callback>(callback);

        if (::EnumDisplayMonitors(hdc, lprcClip, Implementation::EnumDisplayMonitorsWorker<Callback>, std::bit_cast<LPARAM>(&localCallback)))
        {
            return S_OK;
        }

        return HRESULT_FROM_WIN32(::GetLastError());
    }
}
