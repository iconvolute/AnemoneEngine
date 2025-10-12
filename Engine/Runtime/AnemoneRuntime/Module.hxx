#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone
{
    class Module_Runtime
    {
    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
    };
}
