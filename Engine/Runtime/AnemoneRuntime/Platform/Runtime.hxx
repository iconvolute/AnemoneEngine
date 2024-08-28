#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Platform
{
    RUNTIME_API void InitializeRuntime(int argc, char** argv);
    RUNTIME_API void FinalizeRuntime();
    RUNTIME_API void SetCrashReportHandling(bool enable);

    class Runtime final
    {
    public:
        Runtime(int argc, char** argv)
        {
            InitializeRuntime(argc, argv);
        }

        Runtime(Runtime const&) = delete;
        Runtime(Runtime&&) = delete;
        Runtime& operator=(Runtime const&) = delete;
        Runtime& operator=(Runtime&&) = delete;

        ~Runtime()
        {
            FinalizeRuntime();
        }
    };
}
