#pragma once
#include "AnemoneInterop/Windows/Headers.hxx"

#include <dwmapi.h>

namespace Anemone::Interop::Windows
{
    inline HRESULT GetWindowNcRenderingEnabled(HWND handle, BOOL& value)
    {
        return DwmGetWindowAttribute(handle, DWMWA_NCRENDERING_ENABLED, &value, sizeof(value));
    }

    inline HRESULT SetWindowNcRenderingPolicy(HWND handle, DWMNCRENDERINGPOLICY value)
    {
        return DwmSetWindowAttribute(handle, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    }

    inline HRESULT SetWindowAllowNcPaint(HWND handle, BOOL value)
    {
        return DwmSetWindowAttribute(handle, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
    }

    inline HRESULT SetWindowCornerPreference(HWND handle, DWM_WINDOW_CORNER_PREFERENCE value)
    {
        return DwmSetWindowAttribute(handle, DWMWA_WINDOW_CORNER_PREFERENCE, &value, sizeof(value));
    }

    inline HRESULT SetWindowUseImmersiveDarkMode(HWND handle, BOOL value)
    {
        return DwmSetWindowAttribute(handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    }

    inline HRESULT SetWindowCaptionColor(HWND hwnd, COLORREF value)
    {
        return DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &value, sizeof(value));
    }

    inline HRESULT IsWindowCloaked(HWND hwnd, DWORD& result)
    {
        return DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &result, sizeof(result));
    }
}
