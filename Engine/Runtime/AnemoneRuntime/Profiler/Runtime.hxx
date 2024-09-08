#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Profiler
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
