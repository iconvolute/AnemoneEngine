#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::Network
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
