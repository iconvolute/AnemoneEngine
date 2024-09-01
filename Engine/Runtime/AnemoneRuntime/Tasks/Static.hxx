#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Tasks
{
    struct TasksStatic final
    {
        static void Initialize();
        static void Finalize();
    };
}
