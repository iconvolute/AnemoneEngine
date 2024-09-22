#pragma once
#include "AnemoneRuntime/App/Window.hxx"

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::App
{
    class WindowsWindow final
        : public Window
    {
    private:
        HWND m_handle{};
        WindowType m_type{WindowType::Game};
        WindowMode m_mode{WindowMode::Windowed};
        float m_dpi{1.0f};
        WINDOWPLACEMENT m_preFullscreenPlacement{};
        uint16_t m_characterHighSurrogate{};
        bool m_resizing{false};
        bool m_inputEnabled{};
        std::optional<HCURSOR> m_cursor{};
        std::optional<Math::SizeF> m_minimumSize;
        std::optional<Math::SizeF> m_maximumSize;

    public:
        WindowsWindow(Application* application, WindowType type);
        WindowsWindow(WindowsWindow const&) = delete;
        WindowsWindow(WindowsWindow&&) = delete;
        WindowsWindow& operator=(WindowsWindow const&) = delete;
        WindowsWindow& operator=(WindowsWindow&&) = delete;
        ~WindowsWindow() override;

        HWND GetHwnd() const
        {
            return this->m_handle;
        }

    public:
        void Close() override;
        void Minimize() override;
        void Maximize() override;
        void Restore() override;
        void BringToFront(bool force) override;
        void Focus() override;
        void SetVisible(bool value) override;
        void SetEnabled(bool value) override;
        void SetInputEnabled(bool value) override;
        bool GetInputEnabled() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool IsFocused() const override;
        bool IsVisible() const override;
        bool IsEnabled() const override;

        WindowMode GetWindowMode() const override;
        void SetWindowMode(WindowMode value) override;

        void SetTitle(std::string_view const& value) override;

        NativeHandle GetNativeHandle() const override;

        Math::RectF GetPlacement() const override;
        void SetPlacement(Math::RectF const& value) override;

        std::optional<Math::SizeF> GetMinimumSize() const override;
        void SetMinimumSize(std::optional<Math::SizeF> const& value) override;

        std::optional<Math::SizeF> GetMaximumSize() const override;
        void SetMaximumSize(std::optional<Math::SizeF> const& value) override;

        Math::RectF GetBounds() const override;
        Math::RectF GetClientBounds() const override;

    public:
        static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam);
    };
}
