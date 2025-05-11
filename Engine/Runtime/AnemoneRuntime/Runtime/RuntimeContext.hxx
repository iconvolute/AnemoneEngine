#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

struct AnemoneRuntimeInitializationContext final
{
    int argc;
    char** argv;
    bool console;
};

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(AnemoneRuntimeInitializationContext const& context);
extern "C" RUNTIME_API void AnemoneRuntimeFinalize();
