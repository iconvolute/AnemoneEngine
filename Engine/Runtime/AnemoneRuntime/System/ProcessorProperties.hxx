#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string_view>

namespace Anemone
{
    enum class ProcessorCharacteristics : uint8_t
    {
        Performance,
        Efficiency,
    };

    struct ProcessorTopology
    {
        ProcessorCharacteristics Characteristics;
        uint32_t PackageId;
        uint32_t CoreId;
        uint32_t ThreadId;
        uint32_t CacheLineSize;

        uint32_t CacheL1;
        uint32_t CacheL2;
        uint32_t CacheL3;
    };

    struct ProcessorProperties final
    {
        ProcessorProperties() = delete;

        RUNTIME_API static size_t GetPhysicalCoresCount();
        RUNTIME_API static size_t GetLogicalCoresCount();
        RUNTIME_API static size_t GetPerformanceCoresCount();
        RUNTIME_API static size_t GetEfficiencyCoresCount();

        RUNTIME_API static bool IsHyperThreadingEnabled();

        RUNTIME_API static size_t GetCacheLineSize();
        RUNTIME_API static size_t GetCacheSizeLevel1();
        RUNTIME_API static size_t GetCacheSizeLevel2();
        RUNTIME_API static size_t GetCacheSizeLevel3();

        RUNTIME_API static std::string_view GetName();
        RUNTIME_API static std::string_view GetVendor();
    };
}
