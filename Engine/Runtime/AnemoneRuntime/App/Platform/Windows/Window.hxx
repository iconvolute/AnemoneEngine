#pragma once
#include "AnemoneRuntime/App/IWindow.hxx"
#include "AnemoneRuntime/Base/Intrusive.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"

namespace Anemone
{
    class Application;
    class IApplicationEvents;

    class Window final
        : public IWindow
        , public IntrusiveListNode<Window>
    {
    public:
        Window(
            Application* application,
            WindowType type,
            WindowMode mode);

        Window(Window const&) = delete;

        Window(Window&&) = delete;

        Window& operator=(Window const&) = delete;

        Window& operator=(Window&&) = delete;

        ~Window() override;

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

        std::optional<Math::SizeF> GetMinimumSize() override;

        void SetMinimumSize(std::optional<Math::SizeF> value) override;

        std::optional<Math::SizeF> GetMaximumSize() override;

        void SetMaximumSize(std::optional<Math::SizeF> value) override;

        void SetCursor(CursorType value) override;

        CursorType GetCursor() override;

        Math::RectF GetBounds() override;

        void SetBounds(Math::RectF value) override;
        
        Math::RectF GetClientBounds() override;

    public:
        HWND GetHandle() const
        {
            return this->_handle;
        }

        IApplicationEvents& GetEvents() const
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

        static Window* GetWindowFromHandle(HWND handle);

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
        Application* _application{};
        IApplicationEvents* _events{};
        HWND _handle{};
        CursorType _cursorType{CursorType::None};
        std::optional<HCURSOR> _cursorHandle{};
        std::optional<Math::SizeF> _minimumSize;
        std::optional<Math::SizeF> _maximumSize;
        bool _inputEnabled{false};
        bool _resizing{false};
        bool _closed{false};
        bool _active{false};
        uint16_t _characterHighSurrogate{};
        WindowType _type{WindowType::Game};
        WindowMode _mode{WindowMode::Windowed};
        WINDOWPLACEMENT _preFullscreenPlacement{};
        Math::RectF _cachedWindowBounds{};
        Math::RectF _cachedClientBounds{};
    };
}
