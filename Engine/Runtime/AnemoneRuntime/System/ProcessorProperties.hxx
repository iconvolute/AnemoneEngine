#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <cstdint>

namespace Anemone
{
    struct ProcessorProperties
    {
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();

        RUNTIME_API static size_t GetPhysicalCoresCount();

        RUNTIME_API static size_t GetLogicalCoresCount();

        RUNTIME_API static size_t GetPerformanceCoresCount();

        RUNTIME_API static size_t GetEfficiencyCoresCount();

        RUNTIME_API static size_t IsHyperThreadingEnabled();

        RUNTIME_API static size_t GetCacheLineSize();

        RUNTIME_API static size_t GetCacheSizeLevel1();

        RUNTIME_API static size_t GetCacheSizeLevel2();

        RUNTIME_API static size_t GetCacheSizeLevel3();

        RUNTIME_API static std::string_view GetProcessorName();

        RUNTIME_API static std::string_view GetProcessorVendor();
    };
}
