#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::System
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
