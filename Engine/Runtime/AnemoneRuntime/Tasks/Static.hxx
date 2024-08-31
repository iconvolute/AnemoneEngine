#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

namespace Anemone::Tasks
{
    void InitializeStatic();
    void ShutdownStatic();
}
