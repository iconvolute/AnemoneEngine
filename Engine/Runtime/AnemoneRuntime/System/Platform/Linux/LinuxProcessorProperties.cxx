#include "AnemoneRuntime/System/Platform/Linux/LinuxProcessorProperties.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    UninitializedObject<LinuxProcessorProperties> GProcessorProperties{};

    LinuxProcessorProperties::LinuxProcessorProperties()
    {
        // This may not be accurate on all systems.
        this->PhysicalCores = sysconf(_SC_NPROCESSORS_CONF);

        cpu_set_t mask;
        CPU_ZERO(&mask);

        if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
        {
            this->LogicalCores = CPU_COUNT(&mask);
        }
        else
        {
            this->LogicalCores = this->PhysicalCores;
        }

        if (this->PerformanceCores == 0)
        {
            this->PerformanceCores = this->PhysicalCores;
            this->EfficiencyCores = 0;
        }
    }
}

namespace Anemone
{
    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return Private::GProcessorProperties->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return Private::GProcessorProperties->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return Private::GProcessorProperties->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return Private::GProcessorProperties->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return Private::GProcessorProperties->HyperThreadingEnabled;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return Private::GProcessorProperties->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return Private::GProcessorProperties->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return Private::GProcessorProperties->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return Private::GProcessorProperties->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return Private::GProcessorProperties->Name;
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return Private::GProcessorProperties->Vendor;
    }
}
