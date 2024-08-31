#include "AnemoneRuntime/System/Static.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"

namespace Anemone::System
{
    UninitializedObject<Environment> GEnvironment;
    UninitializedObject<ProcessorProperties> GProcessorProperties;

    static void InitializeEnvironment(Environment& environment)
    {
        Platform::win32_string_buffer<wchar_t, 512> buffer{};

        // Determine system version
        {
            if (Platform::win32_GetSystemDirectory(buffer))
            {
                std::wstring kernelPath{buffer.as_view()};
                Platform::win32_PathPushFragment(kernelPath, L"kernel32.dll");

                DWORD const dwSize = GetFileVersionInfoSizeW(kernelPath.c_str(), nullptr);

                if (dwSize != 0)
                {
                    std::unique_ptr<std::byte[]> versionInfo = std::make_unique_for_overwrite<std::byte[]>(dwSize);

                    if (GetFileVersionInfoW(kernelPath.c_str(), 0, dwSize, versionInfo.get()) != FALSE)
                    {
                        VS_FIXEDFILEINFO* pFileInfo = nullptr;
                        UINT uLen = 0;

                        if (VerQueryValueW(versionInfo.get(), L"", reinterpret_cast<void**>(&pFileInfo), &uLen) != FALSE)
                        {
                            environment.SystemVersion = fmt::format(
                                "{}.{}.{}.{}",
                                HIWORD(pFileInfo->dwFileVersionMS),
                                LOWORD(pFileInfo->dwFileVersionMS),
                                HIWORD(pFileInfo->dwFileVersionLS),
                                LOWORD(pFileInfo->dwFileVersionLS));
                        }
                    }
                }
            }

            if (environment.SystemVersion.empty())
            {
                environment.SystemVersion = "Unknown";
            }
        }

        // Determine system ID
        {
            Platform::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(Software\Microsoft\Cryptography)", LR"(MachineGuid)");
            Platform::win32_NarrowString(environment.SystemId, buffer.as_view());
        }

        // Executable path
        {
            Platform::win32_QueryFullProcessImageName(buffer);
            Platform::win32_NarrowString(environment.ExecutablePath, buffer.as_view());
            Path::NormalizeDirectorySeparators(environment.ExecutablePath);
        }

        // Startup path
        {
            Platform::win32_GetCurrentDirectory(buffer);
            Platform::win32_NarrowString(environment.StartupPath, buffer.as_view());
            Path::NormalizeDirectorySeparators(environment.StartupPath);
        }

        // Computer name
        {
            Platform::win32_GetComputerName(buffer);
            Platform::win32_NarrowString(environment.ComputerName, buffer.as_view());
        }

        // User name
        {
            Platform::win32_GetUserName(buffer);
            Platform::win32_NarrowString(environment.UserName, buffer.as_view());
        }

        // Profile path
        {
            Platform::win32_GetKnownFolderPath(FOLDERID_Profile, [&](std::wstring_view value)
            {
                Platform::win32_NarrowString(environment.ProfilePath, value);
            });
            Path::NormalizeDirectorySeparators(environment.ProfilePath);
        }

        // Desktop path
        {
            Platform::win32_GetKnownFolderPath(FOLDERID_Desktop, [&](std::wstring_view value)
            {
                Platform::win32_NarrowString(environment.DesktopPath, value);
            });
            Path::NormalizeDirectorySeparators(environment.DesktopPath);
        }

        // Documents path
        {
            Platform::win32_GetKnownFolderPath(FOLDERID_Documents, [&](std::wstring_view value)
            {
                Platform::win32_NarrowString(environment.DocumentsPath, value);
            });
            Path::NormalizeDirectorySeparators(environment.DocumentsPath);
        }

        // Downloads path
        {
            Platform::win32_GetKnownFolderPath(FOLDERID_Downloads, [&](std::wstring_view value)
            {
                Platform::win32_NarrowString(environment.DownloadsPath, value);
            });
            Path::NormalizeDirectorySeparators(environment.DownloadsPath);
        }

        // Temp path
        {
            Platform::win32_GetTempPath(buffer);
            Platform::win32_NarrowString(environment.TempPath, buffer.as_view());

            Platform::win32_string_buffer<wchar_t, MAX_PATH + 1> tempPath{};
            Platform::win32_GetLongPathName(tempPath, buffer.data());
            Platform::win32_NarrowString(environment.TempPath, tempPath.as_view());
            Path::NormalizeDirectorySeparators(environment.TempPath);
        }

        // Startup time
        {
            environment.StartupTime = DateTime::Now();
        }
    }

    static void InitializeProcessorProperties(ProcessorProperties& properties)
    {
        // Determine Cache properties.
        {
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

                    cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                    switch (current.Cache.Level)
                    {
                    case 1:
                        properties.CacheSizeLevel1 += current.Cache.CacheSize;
                        break;

                    case 2:
                        properties.CacheSizeLevel2 += current.Cache.CacheSize;
                        break;

                    case 3:
                        properties.CacheSizeLevel3 += current.Cache.CacheSize;
                        break;

                    case 4:
                        properties.CacheSizeLevel4 += current.Cache.CacheSize;
                        break;

                    default:
                        // Unknown cache line level. Are you some kind of server from the future?
                        break;
                    }

                    it += current.Size;
                }

                properties.CacheLineSize = cacheLineSize;
            }
        }

        // Determine CPUs
        {
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
                                ++properties.EfficiencyCores;
                            }
                            else
                            {
                                ++properties.PerformanceCores;
                            }

                            ++properties.PhysicalCores;
                        }

                        ++properties.LogicalCores;
                        break;
                    }

                    it += current.Size;
                }

                if (properties.PerformanceCores == 0)
                {
                    // Non-hybrid CPUs report all cores as efficient cores.
                    properties.PerformanceCores = properties.EfficiencyCores;
                    properties.EfficiencyCores = 0;
                }
            }
        }

        // Query CPU name from registry
        {
            Platform::win32_string_buffer<wchar_t, 128> buffer{};

            Platform::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(ProcessorNameString)");
            Platform::win32_NarrowString(properties.Name, buffer.as_view());

            Platform::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(VendorIdentifier)");
            Platform::win32_NarrowString(properties.Vendor, buffer.as_view());
        }
        
    }

    void InitializeStatic()
    {
        GEnvironment.Create();
        InitializeEnvironment(GEnvironment.Get());

        GProcessorProperties.Create();
        InitializeProcessorProperties(GProcessorProperties.Get());
    }

    void ShutdownStatic()
    {
        GProcessorProperties.Destroy();
        GEnvironment.Destroy();
    }
}
