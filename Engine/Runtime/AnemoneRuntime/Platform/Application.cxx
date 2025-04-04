#include "AnemoneRuntime/Platform/Application.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    IApplicationEvents* IApplicationEvents::GCurrent = nullptr;

    
    IApplicationEvents::IApplicationEvents()
    {
        if (IApplicationEvents::GCurrent != nullptr)
        {
            AE_PANIC("Application Events already initialized");
        }

        IApplicationEvents::GCurrent = this;
    }

    IApplicationEvents::~IApplicationEvents()
    {
        if (IApplicationEvents::GCurrent != this)
        {
            AE_PANIC("Application Events not initialized");
        }

        IApplicationEvents::GCurrent = nullptr;
    }
}
