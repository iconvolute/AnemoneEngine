#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Memory
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
