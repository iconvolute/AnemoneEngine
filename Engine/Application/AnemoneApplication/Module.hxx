#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone
{
    template <typename Module>
    struct ModuleInitializer final
    {
        ModuleInitializer()
        {
            Module::Initialize();
        }

        ~ModuleInitializer()
        {
            Module::Finalize();
        }
    };

    class Module_Application
    {
    public:
        APPLICATION_API static void Initialize();
        APPLICATION_API static void Finalize();
    };
}
