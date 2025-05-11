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

    private:
        bool ValidateState() const
        {
            return (this->_handle != nullptr);
        }

    private:
        void RegisterWindowClass();

        static Window* GetWindowFromHandle(HWND handle);

        static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

        static Interop::Windows::WindowMessageResult WmNcCreate(HWND window, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmClose(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmChar(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmDpiChanged(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmPowerBroadcast(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmDisplayChange(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmMenuChar(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmGetDlgCode(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmEnterSizeMove(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmExitSizeMove(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmSize(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmGetMinMaxInfo(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmNcCalcSize(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmActivate(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmActivateApp(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmEndSession(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmSetCursor(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmSysCommand(WPARAM wparam, LPARAM lparam);
        Interop::Windows::WindowMessageResult WmEraseBackground(WPARAM wparam, LPARAM lparam);

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
