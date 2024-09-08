#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Platform
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
