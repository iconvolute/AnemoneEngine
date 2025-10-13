#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone
{
    struct Module_Tasks
    {
        TASKS_API static void Initialize();
        TASKS_API static void Finalize();
    };
}
