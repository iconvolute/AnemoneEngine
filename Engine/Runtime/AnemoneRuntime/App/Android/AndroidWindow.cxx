#include "AnemoneRuntime/App/Android/AndroidWindow.hxx"

namespace Anemone::App
{
    AndroidWindow::AndroidWindow(Application* application, WindowType type)
        : Window{application}
        , m_type{type}
    {
    }

    AndroidWindow::~AndroidWindow() = default;

    void AndroidWindow::Close()
    {
    }

    void AndroidWindow::Minimize()
    {
    }

    void AndroidWindow::Maximize()
    {
    }

    void AndroidWindow::Restore()
    {
    }

    void AndroidWindow::BringToFront(bool force)
    {
        (void)force;
    }

    void AndroidWindow::Focus()
    {
    }

    void AndroidWindow::SetVisible(bool value)
    {
        (void)value;
    }

    void AndroidWindow::SetEnabled(bool value)
    {
        (void)value;
    }

    void AndroidWindow::SetInputEnabled(bool value)
    {
        (void)value;
    }

    bool AndroidWindow::GetInputEnabled() const
    {
        return false;
    }

    bool AndroidWindow::IsMinimized() const
    {
        return false;
    }

    bool AndroidWindow::IsMaximized() const
    {
        return false;
    }

    bool AndroidWindow::IsFocused() const
    {
        return false;
    }

    bool AndroidWindow::IsVisible() const
    {
        return false;
    }

    bool AndroidWindow::IsEnabled() const
    {
        return false;
    }

    WindowMode AndroidWindow::GetWindowMode() const
    {
        return this->m_mode;
    }

    void AndroidWindow::SetWindowMode(WindowMode value)
    {
        this->m_mode = value;
    }

    void AndroidWindow::SetTitle(std::string_view const& value)
    {
        (void)value;
    }

    Window::NativeHandle AndroidWindow::GetNativeHandle() const
    {
        return {};
    }

    Math::RectF AndroidWindow::GetPlacement() const
    {
        return Math::RectF::CreateEmpty();
    }

    void AndroidWindow::SetPlacement(Math::RectF const& value)
    {
        (void)value;
    }

    std::optional<Math::SizeF> AndroidWindow::GetMinimumSize() const
    {
        return {};
    }

    void AndroidWindow::SetMinimumSize(std::optional<Math::SizeF> const& value)
    {
        (void)value;
    }

    std::optional<Math::SizeF> AndroidWindow::GetMaximumSize() const
    {
        return {};
    }

    void AndroidWindow::SetMaximumSize(std::optional<Math::SizeF> const& value)
    {
        (void)value;
    }

    Math::RectF AndroidWindow::GetBounds() const
    {
        return Math::RectF::CreateEmpty();
    }

    Math::RectF AndroidWindow::GetClientBounds() const
    {
        return Math::RectF::CreateEmpty();
    }
}
