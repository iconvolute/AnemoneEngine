#include "AnemoneRuntime/System/Platform/Windows/WindowsProcessorProperties.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/MemoryBuffer.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"

#include <VersionHelpers.h>
#include <algorithm>
#include <vector>

namespace Anemone::Internal
{
    UninitializedObject<WindowsProcessorProperties> GProcessorProperties{};

    struct CoreInfoImpl
    {
        KAFFINITY Mask{};
        WORD Group{};
        uint8_t Efficiency{};
        bool HyperThreaded{};
        bool Performance{};
    };

    WindowsProcessorProperties::WindowsProcessorProperties()
    {
        Interop::memory_buffer<4096> buffer{};

        {
            // Determine Cache properties.
            DWORD dwSize{};
            GetLogicalProcessorInformationEx(RelationAll, nullptr, &dwSize);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                buffer.resize_for_override(dwSize);

                GetLogicalProcessorInformationEx(
                    RelationAll,
                    reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.data()),
                    &dwSize);

                std::byte const* first = buffer.data();
                std::byte const* last = buffer.data() + dwSize;
                std::byte const* it = first;

                uint32_t cacheLineSize = std::numeric_limits<uint32_t>::max();

                bool hasEfficiency = IsWindows10OrGreater();

                std::vector<CoreInfoImpl> coreInfo;
                uint8_t performanceEfficiencyClass = 0;

                while (it < last)
                {
                    auto const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(it);

                    switch (current.Relationship)
                    {
                    case RelationCache:
                        {
                            // Choose the smallest cache line size as the "safest" value.
                            cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                            switch (current.Cache.Level)
                            {
                            case 1:
                                this->CacheSizeLevel1 += current.Cache.CacheSize;
                                break;

                            case 2:
                                this->CacheSizeLevel2 += current.Cache.CacheSize;
                                break;

                            case 3:
                                this->CacheSizeLevel3 += current.Cache.CacheSize;
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
                                        this->HyperThreadingEnabled = true;
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

                    it += current.Size;
                }

                this->CacheLineSize = cacheLineSize;

                for (auto& core : coreInfo)
                {
                    if (core.Efficiency == performanceEfficiencyClass)
                    {
                        core.Performance = true;
                    }
                    else
                    {
                        core.Performance = false;
                    }
                }

                this->LogicalCores = 0;
                this->PhysicalCores = 0;
                this->PerformanceCores = 0;
                this->EfficiencyCores = 0;

                for (auto const& core : coreInfo)
                {
                    // Classify core by efficiency.
                    if (core.Performance)
                    {
                        ++this->PerformanceCores;
                    }
                    else
                    {
                        ++this->EfficiencyCores;
                    }

                    // Count logical cores.
                    ++this->LogicalCores;

                    if (core.HyperThreaded)
                    {
                        ++this->LogicalCores;
                    }

                    // And physical cores.
                    ++this->PhysicalCores;
                }
            }
        }

        if (auto const key = Interop::Windows::RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)"))
        {
            // Query CPU name from registry
            Interop::string_buffer<wchar_t, 128> stringBuffer{};

            if (key.ReadString(LR"(ProcessorNameString)", stringBuffer))
            {
                Interop::Windows::NarrowString(this->Name, stringBuffer.as_view());
            }
            else
            {
                Debugger::ReportApplicationStop("Cannot obtain processor information");
            }

            if (key.ReadString(LR"(VendorIdentifier)", stringBuffer))
            {
                Interop::Windows::NarrowString(this->Vendor, stringBuffer.as_view());
            }
            else
            {
                Debugger::ReportApplicationStop("Cannot obtain processor information");
            }
        }
        else
        {
            Debugger::ReportApplicationStop("Cannot obtain processor information");
        }
    }
}

namespace Anemone
{
    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return Internal::GProcessorProperties->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return Internal::GProcessorProperties->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return Internal::GProcessorProperties->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return Internal::GProcessorProperties->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return Internal::GProcessorProperties->HyperThreadingEnabled;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return Internal::GProcessorProperties->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return Internal::GProcessorProperties->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return Internal::GProcessorProperties->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return Internal::GProcessorProperties->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return Internal::GProcessorProperties->Name;
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return Internal::GProcessorProperties->Vendor;
    }
}
