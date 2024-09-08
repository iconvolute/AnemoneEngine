#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Memory
{
    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;
    }
}
