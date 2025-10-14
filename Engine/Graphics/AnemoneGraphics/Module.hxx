#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone
{
    class Module_Graphics
    {
    public:
        GRAPHICS_API static void Initialize();
        GRAPHICS_API static void Finalize();
    };
}
