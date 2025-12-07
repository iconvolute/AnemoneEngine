#pragma once
#include "AnemoneRuntime.HostApplication/HostWindow.hxx"
#include "AnemoneRuntime.Base/Intrusive.hxx"

namespace Anemone
{
    class HostApplication;
    class HostApplicationEvents;

    class LinuxHostApplication;

    class LinuxHostWindow final
        : public HostWindow
        , public IntrusiveListNode<LinuxHostWindow>
    {
    public:
        LinuxHostWindow(
            LinuxHostApplication& application,
            WindowType type,
            WindowMode mode);

        LinuxHostWindow(LinuxHostWindow const&) = delete;

        LinuxHostWindow(LinuxHostWindow&&) = delete;

        ~LinuxHostWindow() override;

        LinuxHostWindow& operator=(LinuxHostWindow const&) = delete;

        LinuxHostWindow& operator=(LinuxHostWindow&&) = delete;

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
        HostApplicationEvents& GetEvents() const
        {
            return *this->_events;
        }

    private:
        LinuxHostApplication* _application{};
        HostApplicationEvents* _events{};
        WindowType _windowType;
        WindowMode _windowMode;
    };
}
