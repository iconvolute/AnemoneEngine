#include "AnemoneApplication/Platform/Linux/LinuxHostWindow.hxx"
#include "AnemoneApplication/Platform/Linux/LinuxHostApplication.hxx"
#include "AnemoneApplication/HostApplicationEvents.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

namespace Anemone
{
    LinuxHostWindow::LinuxHostWindow(
        LinuxHostApplication& application,
        WindowType windowType,
        WindowMode windowMode)
        : _application{&application}
        , _events{application.events}
        , _windowType{windowType}
        , _windowMode{windowMode}
    {
        AE_PANIC("Not implemented");
        this->_application->windows.PushBack(this);
    }

    LinuxHostWindow::~LinuxHostWindow()
    {
        AE_PANIC("Not implemented");
        this->_application->windows.Remove(this);
    }

    void* LinuxHostWindow::GetNativeHandle() const
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::Close()
    {
        AE_PANIC("Not implemented");
    }

    bool LinuxHostWindow::IsClosed()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::Minimize()
    {
        AE_PANIC("Not implemented");
    }

    bool LinuxHostWindow::IsMinimized()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::Maximize()
    {
        AE_PANIC("Not implemented");
    }

    bool LinuxHostWindow::IsMaximized()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::Restore()
    {
        AE_PANIC("Not implemented");
    }

    void LinuxHostWindow::BringToFront(bool force)
    {
        AE_PANIC("Not implemented");
        (void)force;
    }

    void LinuxHostWindow::Focus()
    {
        AE_PANIC("Not implemented");
    }

    bool LinuxHostWindow::IsFocused()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    bool LinuxHostWindow::IsActive()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::Activate()
    {
        AE_PANIC("Not implemented");
    }

    void LinuxHostWindow::SetVisible(bool value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    bool LinuxHostWindow::IsVisible()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::SetEnabled(bool value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    bool LinuxHostWindow::IsEnabled()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::SetInputEnabled(bool value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    bool LinuxHostWindow::GetInputEnabled()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    WindowMode LinuxHostWindow::GetMode()
    {
        AE_PANIC("Not implemented");
        return this->_windowMode;
    }

    void LinuxHostWindow::SetMode(WindowMode value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    void LinuxHostWindow::SetTitle(std::string_view value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    std::optional<Math::SizeF> LinuxHostWindow::GetMinimumSize()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::SetMinimumSize(std::optional<Math::SizeF> value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    std::optional<Math::SizeF> LinuxHostWindow::GetMaximumSize()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::SetMaximumSize(std::optional<Math::SizeF> value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    void LinuxHostWindow::SetCursor(CursorType value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    CursorType LinuxHostWindow::GetCursor()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    Math::RectF LinuxHostWindow::GetBounds()
    {
        AE_PANIC("Not implemented");
        return {};
    }

    void LinuxHostWindow::SetBounds(Math::RectF value)
    {
        AE_PANIC("Not implemented");
        (void)value;
    }

    Math::RectF LinuxHostWindow::GetClientBounds()
    {
        AE_PANIC("Not implemented");
        return {};
    }
}
