#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <string_view>
#include <cstdint>

namespace Anemone
{
    struct ProcessorProperties
    {
        ANEMONE_RUNTIME_BASE_API static void Initialize();
        ANEMONE_RUNTIME_BASE_API static void Finalize();

        ANEMONE_RUNTIME_BASE_API static size_t GetPhysicalCoresCount();

        ANEMONE_RUNTIME_BASE_API static size_t GetLogicalCoresCount();

        ANEMONE_RUNTIME_BASE_API static size_t GetPerformanceCoresCount();

        ANEMONE_RUNTIME_BASE_API static size_t GetEfficiencyCoresCount();

        ANEMONE_RUNTIME_BASE_API static size_t IsHyperThreadingEnabled();

        ANEMONE_RUNTIME_BASE_API static size_t GetCacheLineSize();

        ANEMONE_RUNTIME_BASE_API static size_t GetCacheSizeLevel1();

        ANEMONE_RUNTIME_BASE_API static size_t GetCacheSizeLevel2();

        ANEMONE_RUNTIME_BASE_API static size_t GetCacheSizeLevel3();

        ANEMONE_RUNTIME_BASE_API static std::string_view GetProcessorName();

        ANEMONE_RUNTIME_BASE_API static std::string_view GetProcessorVendor();
    };
}
