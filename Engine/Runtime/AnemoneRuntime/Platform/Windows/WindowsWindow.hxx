#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseWindow.hxx"

namespace Anemone
{
    class WindowsWindow final
        : public Window
    {
    private:
        HWND m_windowHandle{};
        CursorType m_cursorType{CursorType::None};
        std::optional<HCURSOR> m_cursorHandle{};
        std::optional<Math::SizeF> m_minimumSize;
        std::optional<Math::SizeF> m_maximumSize;
        bool m_inputEnabled{};
        bool m_resizing{};
        uint16_t m_characterHighSurrogate{};
        float m_dpi{1.0f};
        WindowMode m_mode{WindowMode::Windowed};
        WindowType m_type{WindowType::Game};
        WINDOWPLACEMENT m_preFullscreenPlacement{};

    public:
        WindowsWindow(WindowType type);
        WindowsWindow(WindowsWindow const&) = delete;
        WindowsWindow(WindowsWindow&&) = delete;
        WindowsWindow& operator=(WindowsWindow const&) = delete;
        WindowsWindow& operator=(WindowsWindow&&) = delete;
        ~WindowsWindow() override;

        HWND GetNativeHandle() const
        {
            return this->m_windowHandle;
        }

    public:
        void Close() override;

        void Minimize() override;
        bool IsMinimized() const override;

        void Maximize() override;
        bool IsMaximized() const override;

        void Restore() override;
        void BringToFront(bool force) override;

        void Focus() override;
        bool IsFocused() const override;

        void SetVisible(bool value) override;
        bool IsVisible() const override;

        void SetEnabled(bool value) override;
        bool IsEnabled() const override;

        void SetInputEnabled(bool value) override;
        bool GetInputEnabled() const override;

        WindowMode GetMode() const override;
        void SetMode(WindowMode value) override;

        void SetTitle(std::string_view value) override;

        Math::RectF GetPlacement() const override;
        void SetPlacement(Math::RectF value) override;

        std::optional<Math::SizeF> GetMinimumSize() const override;
        void SetMinimumSize(std::optional<Math::SizeF> value) override;

        std::optional<Math::SizeF> GetMaximumSize() const override;
        void SetMaximumSize(std::optional<Math::SizeF> value) override;

        void SetCursor(CursorType value) override;
        CursorType GetCursor() const override;

        Math::RectF GetBounds() const override;
        Math::RectF GetClientBounds() const override;

    public:
        static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);        
    };
}
