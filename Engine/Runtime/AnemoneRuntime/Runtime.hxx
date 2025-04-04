#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone
{
    struct Runtime
    {
        Runtime() = delete;

        RUNTIME_API static void Initialize(int argc, char** argv);
        RUNTIME_API static void Finalize();
    };
}
