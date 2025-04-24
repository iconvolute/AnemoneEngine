#pragma once
#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string>

namespace Anemone::Internal
{
    struct LinuxProcessorProperties final
    {
        // Number of physical cores.
        uint32_t PhysicalCores{};

        // Number of logical cores.
        uint32_t LogicalCores{};

        // Number of performance cores.
        uint32_t PerformanceCores{};

        // Number of efficiency cores.
        uint32_t EfficiencyCores{};

        bool HyperThreadingEnabled{};

        // Smallest cache-line size found.
        uint32_t CacheLineSize{};

        uint32_t CacheSizeLevel1{};
        uint32_t CacheSizeLevel2{};
        uint32_t CacheSizeLevel3{};

        std::string Name{};
        std::string Vendor{};

        LinuxProcessorProperties();
    };

    extern UninitializedObject<LinuxProcessorProperties> GProcessorProperties;
}
