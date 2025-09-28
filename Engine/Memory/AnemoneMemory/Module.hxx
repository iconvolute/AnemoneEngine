#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone
{
    class Module_Memory
    {
    public:
        MEMORY_API static void Initialize();
        MEMORY_API static void Finalize();
    };
}
