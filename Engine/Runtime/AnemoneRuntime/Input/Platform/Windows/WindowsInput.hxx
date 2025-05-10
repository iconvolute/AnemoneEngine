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
        static void Initialize();
        static void Finalize();
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

        bool ProcessMessage(
            IApplicationEvents& events,
            Interop::Windows::WindowMessageResult& result,
            Window& window,
            UINT message,
            WPARAM wparam,
            LPARAM lparam);

        Interop::Windows::WindowMessageResult WmInput(
            IApplicationEvents& events,
            Window& window,
            WPARAM wparam,
            LPARAM lparam);

        Interop::Windows::WindowMessageResult WmKey(
            IApplicationEvents& events,
            Window& window,
            WPARAM wparam,
            LPARAM lparam,
            BOOL pressed);

        Interop::Windows::WindowMessageResult WmMouseButton(
            IApplicationEvents& events,
            Window& window,
            VirtualKey virtualKey,
            WPARAM wparam,
            LPARAM lparam,
            BOOL pressed,
            BOOL click);

        Interop::Windows::WindowMessageResult WmMouseMove(
            IApplicationEvents& events,
            Window& window,
            WPARAM wparam,
            LPARAM lparam);

        Interop::Windows::WindowMessageResult WmMouseWheel(
            IApplicationEvents& events,
            Window& window,
            WPARAM wparam,
            LPARAM lparam);

        Interop::Windows::WindowMessageResult WmMouseHWheel(
            IApplicationEvents& events,
            Window& window,
            WPARAM wparam,
            LPARAM lparam);

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
