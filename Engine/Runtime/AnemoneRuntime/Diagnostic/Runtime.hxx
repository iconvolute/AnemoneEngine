#pragma once
#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Diagnostic
{
    void InitializeRuntime(RuntimeInitializeContext& context);
    void FinalizeRuntime(RuntimeFinalizeContext& context);
}
