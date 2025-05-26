#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Input/Types.hxx"
#include "AnemoneRuntime/App/IApplicationEvents.hxx"

namespace Anemone
{
    class Window;
}

namespace Anemone
{
    class WindowsInput final
    {
    public:
        static WindowsInput& Get();

    public:
        WindowsInput() = default;
        WindowsInput(WindowsInput const&) = delete;
        WindowsInput(WindowsInput&&) = delete;
        WindowsInput& operator=(WindowsInput const&) = delete;
        WindowsInput& operator=(WindowsInput&&) = delete;
        ~WindowsInput() = default;

    public:
        void NotifyWindowActivated(Window& window);

        void NotifyWindowDeactivated(Window& window);

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
            Window& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmInput(
            Window& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmKey(
            Window& window,
            Interop::Windows::WindowMessage const& message,
            BOOL pressed);

        Interop::Windows::WindowMessageResult WmMouseButton(
            Window& window,
            Interop::Windows::WindowMessage const& message,
            VirtualKey virtualKey,
            BOOL pressed,
            BOOL click);

        Interop::Windows::WindowMessageResult WmMouseMove(
            Window& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmMouseWheel(
            Window& window,
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmMouseHWheel(
            Window& window,
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
