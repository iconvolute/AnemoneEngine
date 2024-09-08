#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Tasks
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
