#pragma once
#include "AnemoneRuntime/Runtime.hxx"

namespace Anemone::App
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
