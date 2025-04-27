#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/System/Application.hxx"

namespace Anemone::Internal::Windows
{
    class WindowImpl;

    // Implements input management for Windows.
    class InputImpl final
    {
    private:
        VirtualKeyModifiers m_KeyModifiers{};

    public:
        InputImpl() = default;
        InputImpl(InputImpl const&) = delete;
        InputImpl(InputImpl&&) = delete;
        InputImpl& operator=(InputImpl const&) = delete;
        InputImpl& operator=(InputImpl&&) = delete;
        ~InputImpl() = default;

    public:
        void NotifyWindowActivated(WindowImpl& window);
        void NotifyWindowDeactivated(WindowImpl& window);
        void NotifyApplicationActivated();
        void NotifyApplicationDeactivated();

    public:

        VirtualKeyModifiers GetKeyModifiers() const
        {
            return this->m_KeyModifiers;
        }

        bool StartTracking(HWND handle);

        void StopTracking(HWND handle);

        bool IsTracking(HWND handle) const
        {
            return this->m_TrackMouseWindowHandle == handle;
        }

        bool ProcessMessage(Interop::Windows::WindowMessageResult& result, WindowImpl& window, UINT message, WPARAM wparam, LPARAM lparam);

        Interop::Windows::WindowMessageResult WmInput(WindowImpl& window, WPARAM wparam, LPARAM lparam);

        Interop::Windows::WindowMessageResult WmKey(WindowImpl& window, WPARAM wparam, LPARAM lparam, BOOL pressed);

        Interop::Windows::WindowMessageResult WmMouseButton(WindowImpl& window, VirtualKey virtualKey, WPARAM wparam, LPARAM lparam, BOOL pressed, BOOL click);

        Interop::Windows::WindowMessageResult WmMouseMove(WindowImpl& window, WPARAM wparam, LPARAM lparam);

        Interop::Windows::WindowMessageResult WmMouseWheel(WindowImpl& window, WPARAM wparam, LPARAM lparam);

        Interop::Windows::WindowMessageResult WmMouseHWheel(WindowImpl& window, WPARAM wparam, LPARAM lparam);

    private:
        static VirtualKeyModifiers CaptureModifiers();
        static MouseButtons ConvertToMouseButtons(UINT keyState);

    private:
        HWND m_TrackMouseWindowHandle{};
        POINT m_TrackMouseRestorePosition{};
        POINT m_TrackMouseWindowPosition{};
        POINT m_RawInputLastCursorPosition{};
    };
}
