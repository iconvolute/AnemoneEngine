#pragma once
#include "AnemoneApplication/HostApplicationEvents.hxx"
#include "AnemoneInterop/Windows/UI.hxx"

namespace Anemone
{
    class WindowsHostWindow;

    class WindowsHostInput final
    {
    public:
        static void Initialize();
        static void Finalize();
        static WindowsHostInput& Get();

    public:
        WindowsHostInput() = default;
        WindowsHostInput(WindowsHostInput const&) = delete;
        WindowsHostInput(WindowsHostInput&&) = delete;
        WindowsHostInput& operator=(WindowsHostInput const&) = delete;
        WindowsHostInput& operator=(WindowsHostInput&&) = delete;
        ~WindowsHostInput() = default;

    public:
        void NotifyWindowActivated(WindowsHostWindow& window);

        void NotifyWindowDeactivated(WindowsHostWindow& window);

        void NotifyApplicationActivated();

        void NotifyApplicationDeactivated();

    public:
        VirtualKeyModifiers GetKeyModifiers() const
        {
            return this->_keyModifiers;
        }

        bool StartTracking(HWND handle);

        void StopTracking(HWND handle);

        bool IsTracking(HWND handle) const
        {
            return this->_trackMouseWindowHandle == handle;
        }

        Interop::Windows::WindowMessageResult ProcessMessage(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmInput(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmKey(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message,
            BOOL pressed);

        Interop::Windows::WindowMessageResult WmMouseButton(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message,
            VirtualKey virtualKey,
            BOOL pressed,
            BOOL click);

        Interop::Windows::WindowMessageResult WmMouseMove(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmMouseWheel(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmMouseHWheel(
            WindowsHostWindow& window,
            Interop::Windows::WindowMessage const& message);

    private:
        static VirtualKeyModifiers CaptureModifiers();
        static MouseButtons ConvertToMouseButtons(UINT keyState);

    private:
        HWND _trackMouseWindowHandle{};
        POINT _trackMouseRestorePosition{};
        POINT _trackMouseWindowPosition{};
        POINT _rawInputLastCursorPosition{};

        VirtualKeyModifiers _keyModifiers{};
    };
}
