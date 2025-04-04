#include "AnemoneRuntime/Platform/ProcessorProperties.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone
{
    struct WindowsProcessorProperties final
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

        WindowsProcessorProperties()
        {
            {
                // Determine CPU properties.
                HANDLE hThisProcess = GetCurrentProcess();
                DWORD dwSize{};
                GetSystemCpuSetInformation(nullptr, 0, &dwSize, hThisProcess, 0);

                if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
                {
                    std::unique_ptr<std::byte[]> buffer = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                    GetSystemCpuSetInformation(
                        reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.get()),
                        dwSize,
                        &dwSize,
                        hThisProcess,
                        0);

                    std::byte const* first = buffer.get();
                    std::byte const* last = buffer.get() + dwSize;
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
                    std::unique_ptr<std::byte[]> buffer = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                    GetLogicalProcessorInformationEx(
                        RelationCache,
                        reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.get()),
                        &dwSize);

                    std::byte const* first = buffer.get();
                    std::byte const* last = buffer.get() + dwSize;
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

            // Query CPU name from registry
            Interop::string_buffer<wchar_t, 128> buffer{};

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(ProcessorNameString)");
            Interop::win32_NarrowString(this->Name, buffer.as_view());

            Interop::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(VendorIdentifier)");
            Interop::win32_NarrowString(this->Vendor, buffer.as_view());

        }
    };

    static UninitializedObject<WindowsProcessorProperties> GWindowsProcessorProperties{};
}

namespace Anemone
{
    void ProcessorProperties::Initialize()
    {
        GWindowsProcessorProperties.Create();
    }

    void ProcessorProperties::Finalize()
    {
        GWindowsProcessorProperties.Destroy();
    }

    size_t ProcessorProperties::GetPhysicalCoresCount()
    {
        return GWindowsProcessorProperties->PhysicalCores;
    }

    size_t ProcessorProperties::GetLogicalCoresCount()
    {
        return GWindowsProcessorProperties->LogicalCores;
    }

    size_t ProcessorProperties::GetPerformanceCoresCount()
    {
        return GWindowsProcessorProperties->PerformanceCores;
    }

    size_t ProcessorProperties::GetEfficiencyCoresCount()
    {
        return GWindowsProcessorProperties->EfficiencyCores;
    }

    bool ProcessorProperties::IsHyperThreadingEnabled()
    {
        return GWindowsProcessorProperties->HyperThreadingEnabled;
    }

    size_t ProcessorProperties::GetCacheLineSize()
    {
        return GWindowsProcessorProperties->CacheLineSize;
    }

    size_t ProcessorProperties::GetCacheSizeLevel1()
    {
        return GWindowsProcessorProperties->CacheSizeLevel1;
    }

    size_t ProcessorProperties::GetCacheSizeLevel2()
    {
        return GWindowsProcessorProperties->CacheSizeLevel2;
    }

    size_t ProcessorProperties::GetCacheSizeLevel3()
    {
        return GWindowsProcessorProperties->CacheSizeLevel3;
    }

    std::string_view ProcessorProperties::GetName()
    {
        return GWindowsProcessorProperties->Name;
    }

    std::string_view ProcessorProperties::GetVendor()
    {
        return GWindowsProcessorProperties->Vendor;
    }
}
