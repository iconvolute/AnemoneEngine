#pragma once
#include "AnemoneRuntime/App/IApplication.hxx"

namespace Anemone
{
    class Application final
        : public IApplication
    {
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
    };
}
