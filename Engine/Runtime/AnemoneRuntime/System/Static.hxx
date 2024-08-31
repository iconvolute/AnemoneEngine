#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::System
{
    extern UninitializedObject<Environment> GEnvironment;
    extern UninitializedObject<ProcessorProperties> GProcessorProperties;

    void InitializeStatic();
    void ShutdownStatic();
}
