#include "AnemoneRuntime.System/Platform/Linux/LinuxProcessorProperties.hxx"
#include "AnemoneRuntime.Base/UninitializedObject.hxx"

#include <sys/auxv.h>

#if ANEMONE_ARCHITECTURE_X64
#include <cpuid.h>
#elif ANEMONE_ARCHITECTURE_ARM64
#include <asm/hwcap.h>
#endif

namespace Anemone
{
    namespace
    {
        UninitializedObject<LinuxProcessorProperties> gLinuxProcessorProperties{};
    }

    void LinuxProcessorProperties::Initialize()
    {
        gLinuxProcessorProperties.Create();

        // This may not be accurate on all systems.
        gLinuxProcessorProperties->physicalCores = sysconf(_SC_NPROCESSORS_CONF);

        cpu_set_t mask;
        CPU_ZERO(&mask);

        if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
        {
            gLinuxProcessorProperties->logicalCores = CPU_COUNT(&mask);
        }
        else
        {
            gLinuxProcessorProperties->logicalCores = gLinuxProcessorProperties->physicalCores;
        }

        if (gLinuxProcessorProperties->performanceCores == 0)
        {
            gLinuxProcessorProperties->performanceCores = gLinuxProcessorProperties->physicalCores;
            gLinuxProcessorProperties->efficiencyCores = 0;
        }
    }

    void LinuxProcessorProperties::Finalize()
    {
        gLinuxProcessorProperties.Destroy();
    }

    LinuxProcessorProperties& LinuxProcessorProperties::Get()
    {
        return *gLinuxProcessorProperties;
    }

    void LinuxProcessorProperties::ValidateRequirements()
    {
#if ANEMONE_ARCHITECTURE_X64

        int level0[4]{-1};
        __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

        if (level0[0] < 1)
        {
            Debug::ReportApplicationStop("Application stop: CPU Features not available\n");
        }

        int level1[4]{-1};
        __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

        if (!(level1[2] & bit_CMPXCHG16B))
        {
            Debug::ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
        }

        if (!(level1[2] & bit_POPCNT))
        {
            Debug::ReportApplicationStop("Application stop: POPCNT is not supported\n");
        }

#if ANEMONE_FEATURE_AVX
        if (!(level1[2] & bit_OSXSAVE))
        {
            Debug::ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
        }

        if (!(level1[2] & bit_SSE3))
        {
            Debug::ReportApplicationStop("Application stop: SSE3 is not supported\n");
        }

        if (!(level1[2] & bit_SSE4_1))
        {
            Debug::ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
        }

        if (!(level1[2] & bit_AVX))
        {
            Debug::ReportApplicationStop("Application stop: AVX is not supported\n");
        }

#if ANEMONE_FEATURE_AVX2

        if (!(level1[2] & bit_F16C))
        {
            Debug::ReportApplicationStop("Application stop: F16C is not supported\n");
        }

        if (!(level1[2] & bit_FMA))
        {
            Debug::ReportApplicationStop("Application stop: FMA is not supported\n");
        }

        int level7_0[4]{-1};
        __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

        if (!(level7_0[1] & bit_AVX2))
        {
            Debug::ReportApplicationStop("Application stop: AVX2 is not supported\n");
        }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

        auto const cap = getauxval(AT_HWCAP);

        if ((cap & HWCAP_FP) != HWCAP_FP)
        {
            Debug::ReportApplicationStop("Application stop: NEON not supported\n");
        }

        if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
        {
            Debug::ReportApplicationStop("Application stop: ASIMD not supported\n");
        }

        if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
        {
            Debug::ReportApplicationStop("Application stop: ATOMICS not supported\n");
        }

#endif
    }
}

namespace Anemone
{
    void ProcessorProperties::Initialize()
    {
        LinuxProcessorProperties::Initialize();
    }

    void ProcessorProperties::Finalize()
    {
        LinuxProcessorProperties::Finalize();
    }

    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return gLinuxProcessorProperties->physicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return gLinuxProcessorProperties->logicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return gLinuxProcessorProperties->performanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return gLinuxProcessorProperties->efficiencyCores;
    }

    size_t ProcessorProperties::IsHyperThreadingEnabled()
    {
        return gLinuxProcessorProperties->featureSmt;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return gLinuxProcessorProperties->cacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return gLinuxProcessorProperties->cacheL1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return gLinuxProcessorProperties->cacheL2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return gLinuxProcessorProperties->cacheL3;
    }

    std::string_view ProcessorProperties::GetProcessorName()
    {
        return gLinuxProcessorProperties->processorName.as_view();
    }

    std::string_view ProcessorProperties::GetProcessorVendor()
    {
        return gLinuxProcessorProperties->processorVendor.as_view();
    }
}
