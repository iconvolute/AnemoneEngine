#pragma once
#include "AnemoneInterop/Headers.hxx"


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
}

namespace Anemone
{
    class Module_Runtime
    {
    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
    };

}
