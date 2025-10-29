#pragma once
#include "AnemoneInterop/Headers.hxx"

namespace Anemone
{
    class Module_Render
    {
    public:
        RENDER_API static void Initialize();
        RENDER_API static void Finalize();
    };
}
