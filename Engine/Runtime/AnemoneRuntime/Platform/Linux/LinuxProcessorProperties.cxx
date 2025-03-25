#include "AnemoneRuntime/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone
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

        LinuxProcessorProperties()
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
    };

    static UninitializedObject<LinuxProcessorProperties> GLinuxProcessorProperties{};
}

namespace Anemone
{
    void ProcessorProperties::Initialize()
    {
        GLinuxProcessorProperties.Create();
    }

    void ProcessorProperties::Finalize()
    {
        GLinuxProcessorProperties.Destroy();
    }

    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return GLinuxProcessorProperties->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return GLinuxProcessorProperties->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return GLinuxProcessorProperties->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return GLinuxProcessorProperties->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return GLinuxProcessorProperties->HyperThreadingEnabled;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return GLinuxProcessorProperties->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return GLinuxProcessorProperties->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return GLinuxProcessorProperties->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return GLinuxProcessorProperties->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return GLinuxProcessorProperties->Name;
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return GLinuxProcessorProperties->Vendor;
    }
}
