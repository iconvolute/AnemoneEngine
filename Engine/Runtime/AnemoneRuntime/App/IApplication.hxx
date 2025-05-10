#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/App/IWindow.hxx"

#include <memory>

namespace Anemone
{
    class IApplicationEvents;

    class IApplication
    {
    public:
        IApplication() = default;
        IApplication(IApplication const&) = delete;
        IApplication(IApplication&&) = delete;
        IApplication& operator=(IApplication const&) = delete;
        IApplication& operator=(IApplication&&) = delete;
        virtual ~IApplication() = default;

    public:
        RUNTIME_API static void Initialize(IApplicationEvents* events);
        RUNTIME_API static void Finalize();
        RUNTIME_API static IApplication& Get();

    public:
        virtual void ProcessMessages() = 0;
        virtual std::unique_ptr<IWindow> CreateWindow(WindowType type, WindowMode mode) = 0;
    };
}
