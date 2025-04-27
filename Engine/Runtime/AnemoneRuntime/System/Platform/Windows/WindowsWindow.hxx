#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/System/Window.hxx"
#include "AnemoneRuntime/System/Application.hxx"

namespace Anemone::Internal::Windows
{
    class WindowImpl final
        : public Window
        , public IntrusiveListNode<WindowImpl>
    {
    public:
        explicit WindowImpl(WindowType windowType, WindowMode windowMode);
        WindowImpl(WindowImpl const&) = delete;
        WindowImpl(WindowImpl&&) = delete;
        WindowImpl& operator=(WindowImpl const&) = delete;
        WindowImpl& operator=(WindowImpl&&) = delete;
        ~WindowImpl() override;

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
        void SetBounds(Math::RectF value);
        Math::RectF GetClientBounds() override;

    public:
        HWND GetHandle() const
        {
            return this->m_Handle;
        }

    private:
        bool ValidateState() const;

    private:
        static void RegisterWindowClass();

        static void MapNativeWindowStyle(DWORD& style, DWORD& exStyle, WindowType type, WindowMode mode);

        static WindowImpl* GetWindowFromHandle(HWND handle);
        
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
        HWND m_Handle{};
        CursorType m_CursorType{CursorType::None};
        std::optional<HCURSOR> m_CursorHandle{};
        std::optional<Math::SizeF> m_MinimumSize;
        std::optional<Math::SizeF> m_MaximumSize;
        bool m_InputEnabled{false};
        bool m_Resizing{false};
        bool m_IsClosed{false};
        uint16_t m_CharacterHighSurrogate{};
        WindowType m_Type{WindowType::Game};
        WindowMode m_Mode{WindowMode::Windowed};
        WINDOWPLACEMENT m_PreFullscreenPlacement{};
        Math::RectF m_CachedWindowBounds{};
        Math::RectF m_CachedClientBounds{};
    };
}
