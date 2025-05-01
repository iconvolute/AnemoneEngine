#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Interop/MemoryBuffer.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"

#include <VersionHelpers.h>
#include <algorithm>
#include <vector>

namespace Anemone::Internal::Windows
{
    struct CoreInfoImpl
    {
        KAFFINITY Mask{};
        WORD Group{};
        uint8_t Efficiency{};
        bool HyperThreaded{};
    };

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
        Windows::GProcessorPropertiesStatics.Create();

        Interop::memory_buffer<4096> buffer{};

        if (Interop::Windows::GetLogicalProcessorInformationEx(buffer, RelationAll))
        {
            uint32_t cacheLineSize = std::numeric_limits<uint32_t>::max();

            bool hasEfficiency = IsWindows10OrGreater();

            std::vector<Windows::CoreInfoImpl> coreInfo;
            uint8_t performanceEfficiencyClass = 0;

            std::span view = buffer.as_span();

            while (not view.empty())
            {
                SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(view.data());

                switch (current.Relationship)
                {
                case RelationCache:
                    {
                        // Choose the smallest cache line size as the "safest" value.
                        cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                        switch (current.Cache.Level)
                        {
                        case 1:
                            Windows::GProcessorPropertiesStatics->CacheSizeLevel1 += current.Cache.CacheSize;
                            break;

                        case 2:
                            Windows::GProcessorPropertiesStatics->CacheSizeLevel2 += current.Cache.CacheSize;
                            break;

                        case 3:
                            Windows::GProcessorPropertiesStatics->CacheSizeLevel3 += current.Cache.CacheSize;
                            break;

                        default:
                            // Unknown cache line level. Are you some kind of server from the future?
                            break;
                        }

                        break;
                    }

                case RelationProcessorCore:
                    {
                        if (current.Processor.GroupCount == 1)
                        {
                            // On 64-bit windows, we count only cores on first group.
                            // Maybe in future we enhance this to support more than 64 cores?

                            if (hasEfficiency)
                            {
                                performanceEfficiencyClass = std::max<uint8_t>(performanceEfficiencyClass, current.Processor.EfficiencyClass);

                                if (current.Processor.Flags & LTP_PC_SMT)
                                {
                                    // Hyper-threaded CPU.
                                    Windows::GProcessorPropertiesStatics->HyperThreading = true;
                                }
                            }

                            coreInfo.emplace_back(
                                current.Processor.GroupMask[0].Mask,
                                current.Processor.GroupMask[0].Group,
                                hasEfficiency ? current.Processor.EfficiencyClass : uint8_t{},
                                (current.Processor.Flags & LTP_PC_SMT) != 0);
                        }

                        break;
                    }

                default:
                    {
                        break;
                    }
                }

                view = view.subspan(current.Size);
            }

            Windows::GProcessorPropertiesStatics->CacheLineSize = cacheLineSize;

            Windows::GProcessorPropertiesStatics->LogicalCores = 0;
            Windows::GProcessorPropertiesStatics->PhysicalCores = 0;
            Windows::GProcessorPropertiesStatics->PerformanceCores = 0;
            Windows::GProcessorPropertiesStatics->EfficiencyCores = 0;

            for (auto const& core : coreInfo)
            {
                // Classify core by efficiency.
                if (core.Efficiency == performanceEfficiencyClass)
                {
                    ++Windows::GProcessorPropertiesStatics->PerformanceCores;
                }
                else
                {
                    ++Windows::GProcessorPropertiesStatics->EfficiencyCores;
                }

                // Count logical cores.
                ++Windows::GProcessorPropertiesStatics->LogicalCores;

                if (core.HyperThreaded)
                {
                    ++Windows::GProcessorPropertiesStatics->LogicalCores;
                }

                // And physical cores.
                ++Windows::GProcessorPropertiesStatics->PhysicalCores;
            }
        }

        // Get name and vendor of the CPU
        if (auto const key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)"))
        {
            // Query CPU name from registry
            Interop::string_buffer<wchar_t, 128> stringBuffer{};

            if (key.ReadString(LR"(ProcessorNameString)", stringBuffer))
            {
                Interop::Windows::NarrowString(Windows::GProcessorPropertiesStatics->ProcessorName, stringBuffer.as_view());
            }
            else
            {
                Diagnostics::ReportApplicationStop("Cannot obtain processor information");
            }

            if (key.ReadString(LR"(VendorIdentifier)", stringBuffer))
            {
                Interop::Windows::NarrowString(Windows::GProcessorPropertiesStatics->ProcessorVendor, stringBuffer.as_view());
            }
            else
            {
                Diagnostics::ReportApplicationStop("Cannot obtain processor information");
            }
        }
        else
        {
            Diagnostics::ReportApplicationStop("Cannot obtain processor information");
        }
    }

    extern void FinalizeProcessorProperties()
    {
        Windows::GProcessorPropertiesStatics.Destroy();
    }
}

namespace Anemone
{
    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return Internal::Windows::GProcessorPropertiesStatics->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return Internal::Windows::GProcessorPropertiesStatics->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return Internal::Windows::GProcessorPropertiesStatics->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return Internal::Windows::GProcessorPropertiesStatics->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return Internal::Windows::GProcessorPropertiesStatics->HyperThreading;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return Internal::Windows::GProcessorPropertiesStatics->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return Internal::Windows::GProcessorPropertiesStatics->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return Internal::Windows::GProcessorPropertiesStatics->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return Internal::Windows::GProcessorPropertiesStatics->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return Internal::Windows::GProcessorPropertiesStatics->ProcessorName.as_view();
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return Internal::Windows::GProcessorPropertiesStatics->ProcessorVendor.as_view();
    }
}
