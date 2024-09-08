#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone
{
    RUNTIME_API void InitializeRuntime(int argc, char* argv[]);
    RUNTIME_API void ShutdownRuntime();

    struct RuntimeInitializeContext;
    struct RuntimeFinalizeContext;

    class RuntimeInitializer
    {
    public:
        RuntimeInitializer() = delete;
        RuntimeInitializer(RuntimeInitializer const&) = delete;
        RuntimeInitializer(RuntimeInitializer&&) = delete;
        RuntimeInitializer& operator=(RuntimeInitializer const&) = delete;
        RuntimeInitializer& operator=(RuntimeInitializer&&) = delete;

        RuntimeInitializer(int argc, char* argv[])
        {
            InitializeRuntime(argc, argv);
        }

        ~RuntimeInitializer()
        {
            ShutdownRuntime();
        }
    };
}
