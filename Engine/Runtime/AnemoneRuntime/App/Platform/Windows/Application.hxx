#pragma once
#include "AnemoneRuntime/App/IApplication.hxx"
#include "AnemoneRuntime/App/Platform/Windows/Window.hxx"

namespace Anemone
{
    class Window;

    class Application final
        : public IApplication
    {
        friend class Window;

    public:
        static Application& Get()
        {
            return static_cast<Application&>(IApplication::Get());
        }

    public:
        Application(IApplicationEvents* events);
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;
        ~Application() override = default;

    public:
        void ProcessMessages() override;
        std::unique_ptr<IWindow> CreateWindow(WindowType type, WindowMode mode) override;

    private:
        IApplicationEvents* _events{};
        IntrusiveList<Window> _windows{};

    private:
        HICON _appIcon{};

        HCURSOR _cursorArrow{};
        HCURSOR _cursorArrowWait{};
        HCURSOR _cursorText{};
        HCURSOR _cursorSizeHorizontal{};
        HCURSOR _cursorSizeVertical{};
        HCURSOR _cursorSizeLeft{};
        HCURSOR _cursorSizeTop{};
        HCURSOR _cursorSizeRight{};
        HCURSOR _cursorSizeBottom{};
        HCURSOR _cursorSizeTopLeft{};
        HCURSOR _cursorSizeTopRight{};
        HCURSOR _cursorSizeBottomLeft{};
        HCURSOR _cursorSizeBottomRight{};
        HCURSOR _cursorSizeAll{};
        HCURSOR _cursorCross{};

        [[nodiscard]] HCURSOR GetCursor(CursorType cursor) const;
    };
}
