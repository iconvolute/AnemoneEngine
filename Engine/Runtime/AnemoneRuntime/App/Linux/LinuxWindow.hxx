#pragma once
#include "AnemoneRuntime/App/Window.hxx"

#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

namespace Anemone::App
{
    class LinuxWindow final
        : public Window
    {
    private:
        WindowType m_type{WindowType::Game};
        WindowMode m_mode{WindowMode::Windowed};

    public:
        LinuxWindow(Application* application, WindowType type);
        LinuxWindow(LinuxWindow const&) = delete;
        LinuxWindow(LinuxWindow&&) = delete;
        LinuxWindow& operator=(LinuxWindow const&) = delete;
        LinuxWindow& operator=(LinuxWindow&&) = delete;
        ~LinuxWindow() override;

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
    };
}
