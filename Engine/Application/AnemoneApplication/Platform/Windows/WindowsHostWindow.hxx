#pragma once
#include "AnemoneApplication/HostWindow.hxx"
#include "AnemoneBase/Intrusive.hxx"
#include "AnemoneInterop/Windows/UI.hxx"

namespace Anemone
{
    class HostApplication;
    class HostApplicationEvents;

    class WindowsHostApplication;

    class WindowsHostWindow final
        : public HostWindow
        , public IntrusiveListNode<WindowsHostWindow>
    {
    public:
        WindowsHostWindow(
            WindowsHostApplication& application,
            WindowType type,
            WindowMode mode);

        WindowsHostWindow(WindowsHostWindow const&) = delete;

        WindowsHostWindow(WindowsHostWindow&&) = delete;

        ~WindowsHostWindow() override;

        WindowsHostWindow& operator=(WindowsHostWindow const&) = delete;

        WindowsHostWindow& operator=(WindowsHostWindow&&) = delete;


    public:
        void* GetNativeHandle() const override;

        void Close() override;

        bool IsClosed() override;

        void Minimize() override;

        bool IsMinimized() override;

        void Maximize() override;

        bool IsMaximized() override;

        void Restore() override;

        void BringToFront(bool force) override;

        void Focus() override;

        bool IsFocused() override;

        bool IsActive() override;

        void Activate() override;

        void SetVisible(bool value) override;

        bool IsVisible() override;

        void SetEnabled(bool value) override;

        bool IsEnabled() override;

        void SetInputEnabled(bool value) override;

        bool GetInputEnabled() override;

        WindowMode GetMode() override;

        void SetMode(WindowMode value) override;

        void SetTitle(std::string_view value) override;

        std::optional<SizeF> GetMinimumSize() override;

        void SetMinimumSize(std::optional<SizeF> value) override;

        std::optional<SizeF> GetMaximumSize() override;

        void SetMaximumSize(std::optional<SizeF> value) override;

        void SetCursor(CursorType value) override;

        CursorType GetCursor() override;

        RectF GetBounds() override;

        void SetBounds(RectF value) override;

        RectF GetClientBounds() override;

    public:
        HWND GetHandle() const
        {
            return this->_handle;
        }

        HostApplicationEvents& GetEvents() const
        {
            return *this->_events;
        }

    private:
        bool ValidateState() const
        {
            return (this->_handle != nullptr);
        }

    private:
        void RegisterWindowClass();

        static WindowsHostWindow* GetWindowFromHandle(HWND handle);

        static LRESULT CALLBACK WndProc(
            HWND handle,
            UINT message,
            WPARAM wparam,
            LPARAM lparam);

        static Interop::Windows::WindowMessageResult WmNcCreate(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmClose(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmChar(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmDpiChanged(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmPowerBroadcast(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmDisplayChange(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmMenuChar(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmGetDlgCode(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmEnterSizeMove(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmExitSizeMove(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmSize(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmGetMinMaxInfo(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmNcCalcSize(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmActivate(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmActivateApp(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmEndSession(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmSetCursor(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmSysCommand(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmEraseBackground(
            Interop::Windows::WindowMessage const& message);

        Interop::Windows::WindowMessageResult WmShowWindow(
            Interop::Windows::WindowMessage const& message);

    private:
        WindowsHostApplication* _application{};
        HostApplicationEvents* _events{};
        HWND _handle{};
        CursorType _cursorType{CursorType::None};
        std::optional<HCURSOR> _cursorHandle{};
        std::optional<SizeF> _minimumSize;
        std::optional<SizeF> _maximumSize;
        bool _inputEnabled{false};
        bool _resizing{false};
        bool _closed{false};
        bool _active{false};
        uint16_t _characterHighSurrogate{};
        WindowType _windowType{WindowType::Game};
        WindowMode _windowMode{WindowMode::Windowed};
        WINDOWPLACEMENT _preFullscreenPlacement{};
        RectF _cachedWindowBounds{};
        RectF _cachedClientBounds{};
    };
}
