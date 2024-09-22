#include "AnemoneRuntime/App/Linux/LinuxWindow.hxx"

namespace Anemone::App
{
    LinuxWindow::LinuxWindow(Application* application, WindowType type)
        : Window{application}
        , m_type{type}
    {
    }

    LinuxWindow::~LinuxWindow() = default;

    void LinuxWindow::Close()
    {
    }

    void LinuxWindow::Minimize()
    {
    }

    void LinuxWindow::Maximize()
    {
    }

    void LinuxWindow::Restore()
    {
    }

    void LinuxWindow::BringToFront(bool force)
    {
        (void)force;
    }

    void LinuxWindow::Focus()
    {
    }

    void LinuxWindow::SetVisible(bool value)
    {
        (void)value;
    }

    void LinuxWindow::SetEnabled(bool value)
    {
        (void)value;
    }

    void LinuxWindow::SetInputEnabled(bool value)
    {
        (void)value;
    }

    bool LinuxWindow::GetInputEnabled() const
    {
        return false;
    }

    bool LinuxWindow::IsMinimized() const
    {
        return false;
    }

    bool LinuxWindow::IsMaximized() const
    {
        return false;
    }

    bool LinuxWindow::IsFocused() const
    {
        return false;
    }

    bool LinuxWindow::IsVisible() const
    {
        return false;
    }

    bool LinuxWindow::IsEnabled() const
    {
        return false;
    }

    WindowMode LinuxWindow::GetWindowMode() const
    {
        return this->m_mode;
    }

    void LinuxWindow::SetWindowMode(WindowMode value)
    {
        this->m_mode = value;
    }

    void LinuxWindow::SetTitle(std::string_view const& value)
    {
        (void)value;
    }

    Window::NativeHandle LinuxWindow::GetNativeHandle() const
    {
        return {};
    }

    Math::RectF LinuxWindow::GetPlacement() const
    {
        return Math::RectF::CreateEmpty();
    }

    void LinuxWindow::SetPlacement(Math::RectF const& value)
    {
        (void)value;
    }

    std::optional<Math::SizeF> LinuxWindow::GetMinimumSize() const
    {
        return {};
    }

    void LinuxWindow::SetMinimumSize(std::optional<Math::SizeF> const& value)
    {
        (void)value;
    }

    std::optional<Math::SizeF> LinuxWindow::GetMaximumSize() const
    {
        return {};
    }

    void LinuxWindow::SetMaximumSize(std::optional<Math::SizeF> const& value)
    {
        (void)value;
    }

    Math::RectF LinuxWindow::GetBounds() const
    {
        return Math::RectF::CreateEmpty();
    }

    Math::RectF LinuxWindow::GetClientBounds() const
    {
        return Math::RectF::CreateEmpty();
    }
}
