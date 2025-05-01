#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string>

namespace Anemone::Internal::Linux
{
    struct ProcessorPropertiesStatics final
    {
        // Number of physical cores.
        uint32_t PhysicalCores{};

        // Number of logical cores.
        uint32_t LogicalCores{};

        // Number of performance cores.
        uint32_t PerformanceCores{};

        // Number of efficiency cores.
        uint32_t EfficiencyCores{};

        bool HyperThreading{};

        // Smallest cache-line size found.
        uint32_t CacheLineSize{};

        uint32_t CacheSizeLevel1{};
        uint32_t CacheSizeLevel2{};
        uint32_t CacheSizeLevel3{};

        Interop::string_buffer<char, 64> ProcessorName{};
        Interop::string_buffer<char, 64> ProcessorVendor{};
    };

    static UninitializedObject<ProcessorPropertiesStatics> GProcessorPropertiesStatics{};
}


namespace Anemone::Internal
{
    extern void InitializeProcessorProperties()
    {
        Linux::GProcessorPropertiesStatics.Create();

        // This may not be accurate on all systems.
        Linux::GProcessorPropertiesStatics->PhysicalCores = sysconf(_SC_NPROCESSORS_CONF);

        cpu_set_t mask;
        CPU_ZERO(&mask);

        if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
        {
            Linux::GProcessorPropertiesStatics->LogicalCores = CPU_COUNT(&mask);
        }
        else
        {
            Linux::GProcessorPropertiesStatics->LogicalCores = Linux::GProcessorPropertiesStatics->PhysicalCores;
        }

        if (Linux::GProcessorPropertiesStatics->PerformanceCores == 0)
        {
            Linux::GProcessorPropertiesStatics->PerformanceCores = Linux::GProcessorPropertiesStatics->PhysicalCores;
            Linux::GProcessorPropertiesStatics->EfficiencyCores = 0;
        }
    }

    extern void FinalizeProcessorProperties()
    {
        Linux::GProcessorPropertiesStatics.Destroy();
    }
}

namespace Anemone
{
    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return Internal::Linux::GProcessorPropertiesStatics->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return Internal::Linux::GProcessorPropertiesStatics->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return Internal::Linux::GProcessorPropertiesStatics->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return Internal::Linux::GProcessorPropertiesStatics->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return Internal::Linux::GProcessorPropertiesStatics->HyperThreading;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return Internal::Linux::GProcessorPropertiesStatics->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return Internal::Linux::GProcessorPropertiesStatics->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return Internal::Linux::GProcessorPropertiesStatics->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return Internal::Linux::GProcessorPropertiesStatics->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return Internal::Linux::GProcessorPropertiesStatics->ProcessorName.as_view();
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return Internal::Linux::GProcessorPropertiesStatics->ProcessorVendor.as_view();
    }
}
