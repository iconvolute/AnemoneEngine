#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

#include <fmt/format.h>

namespace Anemone::Diagnostics
{
    class DebugEngine
    {
    public:
        static void Initialize();
        static void Finalize();
        static DebugEngine& Get();

    public:
        DebugEngine() = default;
        DebugEngine(DebugEngine const&) = delete;
        DebugEngine(DebugEngine&&) = delete;
        DebugEngine& operator=(DebugEngine const&) = delete;
        DebugEngine& operator=(DebugEngine&&) = delete;
        virtual ~DebugEngine() = default;

    public:
        virtual void Symbolize(void* address, fmt::memory_buffer& result) = 0;
    };
}
