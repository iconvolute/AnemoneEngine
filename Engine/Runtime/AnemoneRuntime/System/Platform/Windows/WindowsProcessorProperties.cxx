#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsProcessorProperties.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Interop/Windows/MemoryBuffer.hxx"
#include "AnemoneRuntime/Interop/Windows/Process.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

#include <VersionHelpers.h>

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsProcessorProperties> gWindowsProcessorProperties{};
    }

    void WindowsProcessorProperties::Initialize()
    {
        gWindowsProcessorProperties.Create();

        Interop::memory_buffer<4096> buffer{};

        // Find out information about caches.
        if (SUCCEEDED(Interop::Windows::GetLogicalProcessorInformationEx(buffer, RelationCache)))
        {
            size_t cacheLineSize = std::numeric_limits<size_t>::max();
            size_t cacheL1 = 0;
            size_t cacheL2 = 0;
            size_t cacheL3 = 0;

            Interop::Windows::EnumerateLogicalProcessorInformation(buffer.as_span(), [&](SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current)
            {
                AE_ASSERT(current.Relationship == RelationCache);

                // Choose the smallest cache line size as the "safest" value.
                cacheLineSize = std::min<size_t>(cacheLineSize, current.Cache.LineSize);

                switch (current.Cache.Level)
                {
                case 1:
                    cacheL1 += current.Cache.CacheSize;
                    break;

                case 2:
                    cacheL2 += current.Cache.CacheSize;
                    break;

                case 3:
                    cacheL3 += current.Cache.CacheSize;
                    break;

                default:
                    // Unknown cache line level. Are you some kind of server from the future?
                    break;
                }
            });

            gWindowsProcessorProperties->cacheLineSize = cacheLineSize;
            gWindowsProcessorProperties->cacheL1 += cacheL1;
            gWindowsProcessorProperties->cacheL2 += cacheL2;
            gWindowsProcessorProperties->cacheL3 += cacheL3;
        }

        if (SUCCEEDED(Interop::Windows::GetLogicalProcessorInformationEx(buffer, RelationProcessorCore)))
        {
            bool const hasEfficiency = IsWindows10OrGreater();
            bool featureSmt = false;
            BYTE performanceEfficiencyClass = 0;

            // Find out performance CPU efficiency class.
            Interop::Windows::EnumerateLogicalProcessorInformation(buffer.as_span(), [&](SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current)
            {
                AE_ASSERT(current.Relationship == RelationProcessorCore);

                if (hasEfficiency)
                {
                    performanceEfficiencyClass = std::max<uint8_t>(performanceEfficiencyClass, current.Processor.EfficiencyClass);
                }

                if (current.Processor.Flags & LTP_PC_SMT)
                {
                    featureSmt = true;
                }
            });

            gWindowsProcessorProperties->featureSmt = featureSmt;

            // Collect processor core counts.
            size_t logicalCores = 0;
            size_t physicalCores = 0;
            size_t performanceCores = 0;
            size_t efficiencyCores = 0;

            Interop::Windows::EnumerateLogicalProcessorInformation(buffer.as_span(), [&](SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current)
            {
                AE_ASSERT(current.Relationship == RelationProcessorCore);

                if (current.Processor.EfficiencyClass == performanceEfficiencyClass)
                {
                    ++performanceCores;
                }
                else
                {
                    ++efficiencyCores;
                }

                for (size_t i = 0; i < current.Processor.GroupCount; ++i)
                {
                    KAFFINITY const mask = current.Processor.GroupMask[i].Mask;

                    logicalCores += std::popcount(mask);
                }

                ++physicalCores;
            });

            gWindowsProcessorProperties->logicalCores = logicalCores;
            gWindowsProcessorProperties->physicalCores = physicalCores;
            gWindowsProcessorProperties->performanceCores = performanceCores;
            gWindowsProcessorProperties->efficiencyCores = efficiencyCores;
        }

        // Get name and vendor of the CPU
        if (auto key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)"))
        {
            Interop::string_buffer<wchar_t, 128> stringBuffer{};

            if (key.ReadString(LR"(ProcessorNameString)", stringBuffer))
            {
                Interop::Windows::NarrowString(gWindowsProcessorProperties->processorName, stringBuffer.as_view());
            }
            else
            {
                Debug::ReportApplicationStop("Cannot obtain processor information");
            }

            if (key.ReadString(LR"(VendorIdentifier)", stringBuffer))
            {
                Interop::Windows::NarrowString(gWindowsProcessorProperties->processorVendor, stringBuffer.as_view());
            }
            else
            {
                Debug::ReportApplicationStop("Cannot obtain processor information");
            }
        }
        else
        {
            Debug::ReportApplicationStop("Cannot obtain processor information");
        }
    }

    void WindowsProcessorProperties::Finalize()
    {
        gWindowsProcessorProperties.Destroy();
    }

    WindowsProcessorProperties& WindowsProcessorProperties::Get()
    {
        return *gWindowsProcessorProperties;
    }

    void WindowsProcessorProperties::ValidateRequirements()
    {
        if (Interop::Windows::IsProcessEmulated())
        {
            // VERIFY: AVX is not supported in WoA process emulation.
            Debug::ReportApplicationStop("Emulated process not supported.");
        }

        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
        {
            Debug::ReportApplicationStop("64-bit compare-exchange not supported.");
        }

#if ANEMONE_ARCHITECTURE_X64
        if (not IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))
        {
            Debug::ReportApplicationStop("128-bit compare-exchange not supported");
        }
#endif

#if ANEMONE_FEATURE_AVX

        if (not IsProcessorFeaturePresent(PF_XSAVE_ENABLED))
        {
            Debug::ReportApplicationStop("XSAVE not enabled.");
        }

        if (not IsProcessorFeaturePresent(PF_AVX_INSTRUCTIONS_AVAILABLE))
        {
            Debug::ReportApplicationStop("AVX not supported.");
        }
#endif

#if ANEMONE_FEATURE_AVX2
        if (not IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE))
        {
            Debug::ReportApplicationStop("AVX2 not supported.");
        }
#endif

#if ANEMONE_FEATURE_NEON
        if (not IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
        {
            Debug::ReportApplicationStop("NEON not supported.");
        }
#endif

        // Verify Windows version
        OSVERSIONINFOEXW osie{
            .dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW),
            .dwMajorVersion = static_cast<DWORD>(10),
        };

        ULONGLONG conditionMask{};
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MAJORVERSION,
            VER_GREATER_EQUAL);
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MINORVERSION,
            VER_GREATER_EQUAL);

        if (not VerifyVersionInfoW(
                &osie,
                VER_MAJORVERSION | VER_MINORVERSION,
                conditionMask))
        {
            Debug::ReportApplicationStop("Windows 10 or newer required");
        }
    }
}

namespace Anemone
{
    void ProcessorProperties::Initialize()
    {
        WindowsProcessorProperties::Initialize();
    }

    void ProcessorProperties::Finalize()
    {
        WindowsProcessorProperties::Finalize();
    }

    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return gWindowsProcessorProperties->physicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return gWindowsProcessorProperties->logicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return gWindowsProcessorProperties->performanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return gWindowsProcessorProperties->efficiencyCores;
    }

    size_t ProcessorProperties::IsHyperThreadingEnabled()
    {
        return gWindowsProcessorProperties->featureSmt;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return gWindowsProcessorProperties->cacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return gWindowsProcessorProperties->cacheL1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return gWindowsProcessorProperties->cacheL2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return gWindowsProcessorProperties->cacheL3;
    }

    std::string_view ProcessorProperties::GetProcessorName()
    {
        return gWindowsProcessorProperties->processorName.as_view();
    }

    std::string_view ProcessorProperties::GetProcessorVendor()
    {
        return gWindowsProcessorProperties->processorVendor.as_view();
    }
}
