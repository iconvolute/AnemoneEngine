#include "AnemoneRuntime/System/Platform/Windows/WindowsProcessorProperties.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#include "AnemoneRuntime/Interop/MemoryBuffer.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"

namespace Anemone::Internal
{
    UninitializedObject<WindowsProcessorProperties> GProcessorProperties{};

    WindowsProcessorProperties::WindowsProcessorProperties()
    {
        Interop::memory_buffer<4096> buffer{};

        {
            // Determine CPU properties.
            HANDLE hThisProcess = GetCurrentProcess();
            DWORD dwSize{};
            GetSystemCpuSetInformation(nullptr, 0, &dwSize, hThisProcess, 0);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                buffer.resize_for_override(dwSize);

                GetSystemCpuSetInformation(
                    reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.data()),
                    dwSize,
                    &dwSize,
                    hThisProcess,
                    0);

                std::byte const* first = buffer.data();
                std::byte const* last = buffer.data() + dwSize;
                std::byte const* it = first;

                while (it < last)
                {
                    auto const& current = *reinterpret_cast<SYSTEM_CPU_SET_INFORMATION const*>(it);

                    switch (current.Type)
                    {
                    case CpuSetInformation:
                        if (current.CpuSet.CoreIndex == current.CpuSet.LogicalProcessorIndex)
                        {
                            if (current.CpuSet.EfficiencyClass == 0)
                            {
                                ++this->EfficiencyCores;
                            }
                            else
                            {
                                ++this->PerformanceCores;
                            }

                            ++this->PhysicalCores;
                        }

                        ++this->LogicalCores;
                        break;
                    }

                    it += current.Size;
                }

                if (this->PerformanceCores == 0)
                {
                    // Non-hybrid CPUs report all cores as efficient cores.
                    this->PerformanceCores = this->EfficiencyCores;
                    this->EfficiencyCores = 0;
                }

                // Determine Hyper-Threading. CPUs with more logical cores than physical cores are considered hyper-threaded.
                this->HyperThreadingEnabled = this->LogicalCores > this->PhysicalCores;
            }
        }
        {
            // Determine Cache properties.
            DWORD dwSize{};
            GetLogicalProcessorInformationEx(RelationCache, nullptr, &dwSize);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                buffer.resize_for_override(dwSize);

                GetLogicalProcessorInformationEx(
                    RelationCache,
                    reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.data()),
                    &dwSize);

                std::byte const* first = buffer.data();
                std::byte const* last = buffer.data() + dwSize;
                std::byte const* it = first;

                uint32_t cacheLineSize = std::numeric_limits<uint32_t>::max();

                while (it < last)
                {
                    auto const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(it);

                    AE_ASSERT(current.Relationship == RelationCache);

                    // Choose smallest cache line size as the "safest" value.
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

                    it += current.Size;
                }

                this->CacheLineSize = cacheLineSize;
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
