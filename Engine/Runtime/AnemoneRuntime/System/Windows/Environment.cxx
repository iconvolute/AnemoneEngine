#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System
{
    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        result.clear();
        Platform::win32_string_buffer<wchar_t, 128> wname{};
        Platform::win32_WidenString(wname, name);

        if (Platform::win32_string_buffer<wchar_t, 512> wresult{};
            Platform::win32_GetEnvironmentVariable(wresult, wname))
        {
            Platform::win32_NarrowString(result, wresult.as_view());
            return true;
        }

        return false;
    }

    bool SetEnvironmentVariable(std::string_view name, std::string_view value)
    {
        Platform::win32_string_buffer<wchar_t, 128> wname{};
        Platform::win32_WidenString(wname, name);

        Platform::win32_string_buffer<wchar_t, 512> wvalue{};
        Platform::win32_WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname, wvalue) != FALSE;
    }

    bool RemoveEnvironmentVariable(std::string_view name)
    {
        Platform::win32_string_buffer<wchar_t, 128> wname{};
        Platform::win32_WidenString(wname, name);

        return SetEnvironmentVariableW(wname, nullptr) != FALSE;
    }
}

namespace Anemone::System
{
    bool GetClipboardContent(std::string& value)
    {
        value.clear();

        bool result = false;

        if (OpenClipboard(GetActiveWindow()) != FALSE)
        {
            if (HGLOBAL const hUnicodeText = GetClipboardData(CF_UNICODETEXT); hUnicodeText != nullptr)
            {
                wchar_t const* const data = static_cast<wchar_t const*>(GlobalLock(hUnicodeText));
                if (data != nullptr)
                {
                    Platform::win32_NarrowString(value, data);
                    GlobalUnlock(hUnicodeText);
                    result = true;
                }
            }
            else if (HGLOBAL const hText = GetClipboardData(CF_TEXT); hText != nullptr)
            {
                char const* const data = static_cast<char const*>(GlobalLock(hText));
                if (data != nullptr)
                {
                    value = data;
                    GlobalUnlock(hText);
                    result = true;
                }
            }

            CloseClipboard();
        }

        return result;
    }

    bool SetClipboardContent(std::string_view value)
    {
        bool result = false;

        if (OpenClipboard(GetActiveWindow()) != FALSE)
        {
            EmptyClipboard();

            Platform::win32_string_buffer<wchar_t, 512> buffer{};
            Platform::win32_WidenString(buffer, value);
            std::wstring_view const data = buffer.as_view();

            if (HGLOBAL const handle = GlobalAlloc(GMEM_MOVEABLE, data.size() * sizeof(wchar_t)); handle != nullptr)
            {
                if (void* const destination = GlobalLock(handle); destination != nullptr)
                {
                    // Copy zero terminated string.
                    std::memcpy(destination, data.data(), data.size() * sizeof(wchar_t));
                    GlobalUnlock(handle);

                    if (SetClipboardData(CF_UNICODETEXT, handle) != nullptr)
                    {
                        result = true;
                    }
                }
            }

            CloseClipboard();
        }

        return result;
    }
}

namespace Anemone::System
{
    static BOOL CALLBACK FillDisplayDevicesInformation(
        HMONITOR handle,
        HDC,
        LPRECT,
        LPARAM context) noexcept
    {
        DisplayMetrics& displayMetrics = *reinterpret_cast<DisplayMetrics*>(context); // NOLINT(performance-no-int-to-ptr)

        MONITORINFOEXW miex{};
        miex.cbSize = sizeof(miex);

        GetMonitorInfoW(handle, &miex);

        Platform::win32_string_buffer<char, 128> name{};
        Platform::win32_NarrowString(name, miex.szDevice);

        AE_ASSERT(!displayMetrics.Displays.empty());

        DisplayInfo& last = displayMetrics.Displays.back();

        if (last.Name == name.as_view())
        {
            last.DisplayRectangle = Platform::win32_into_Rectangle(miex.rcMonitor);
            last.WorkAreaRectangle = Platform::win32_into_Rectangle(miex.rcWork);
        }

        return TRUE;
    }

    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics.Displays.clear();

        // Get display size.
        displayMetrics.PrimaryDisplaySize = {
            .Width = GetSystemMetrics(SM_CXSCREEN),
            .Height = GetSystemMetrics(SM_CYSCREEN),
        };

        RECT workArea = {-1, -1, -1, -1};

        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) == FALSE)
        {
            workArea = {};
        }

        // Get workspace area.
        displayMetrics.PrimaryDisplayWorkArea = {
            .X = workArea.left,
            .Y = workArea.top,
            .Width = workArea.right - workArea.left,
            .Height = workArea.bottom - workArea.top,
        };

        // Virtual display rect
        displayMetrics.VirtualDisplayRect = {
            .X = GetSystemMetrics(SM_XVIRTUALSCREEN),
            .Y = GetSystemMetrics(SM_YVIRTUALSCREEN),
            .Width = GetSystemMetrics(SM_CXVIRTUALSCREEN),
            .Height = GetSystemMetrics(SM_CYVIRTUALSCREEN),
        };

        // Get displays
        displayMetrics.Displays.clear();

        DISPLAY_DEVICEW device{};
        device.cb = sizeof(device);

        for (DWORD id = 0; EnumDisplayDevicesW(nullptr, id, &device, 0) != FALSE; ++id)
        {
            if ((device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0)
            {
                // Mirroring devices are not supported.
                continue;
            }

            if ((device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
            {
                // Only devices attached to desktop.
                continue;
            }

            DisplayInfo& display = displayMetrics.Displays.emplace_back();
            Platform::win32_NarrowString(display.Id, device.DeviceID);
            Platform::win32_NarrowString(display.Name, device.DeviceName);
            display.Primary = (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODEW dm{};
            dm.dmSize = sizeof(dm);

            if (EnumDisplaySettingsExW(
                    device.DeviceName,
                    ENUM_CURRENT_SETTINGS,
                    &dm,
                    EDS_RAWMODE))
            {
                if (dm.dmFields & DM_DISPLAYORIENTATION)
                {
                    switch (dm.dmDisplayOrientation)
                    {
                    default:
                        display.Orientation = DisplayOrientation::Unknown;
                        break;

                    case DMDO_DEFAULT:
                        display.Orientation = DisplayOrientation::Landscape;
                        break;

                    case DMDO_90:
                        display.Orientation = DisplayOrientation::Portrait;
                        break;

                    case DMDO_180:
                        display.Orientation = DisplayOrientation::LandscapeFlipped;
                        break;

                    case DMDO_270:
                        display.Orientation = DisplayOrientation::PortraitFlipped;
                        break;
                    }
                }
            }
            else
            {
                display.Orientation = DisplayOrientation::Unknown;
            }

            // Find detailed properties.
            EnumDisplayMonitors(
                nullptr,
                nullptr,
                FillDisplayDevicesInformation,
                reinterpret_cast<LPARAM>(&displayMetrics));
        }
    }
}

namespace Anemone::System
{
    ColorRef GetScreenPixel(Point<int32_t> position, float gamma)
    {
        COLORREF const color = GetPixel(GetDC(HWND_DESKTOP), position.X, position.Y);

        float const r = static_cast<float>(static_cast<uint8_t>(color)) / 255.0f;
        float const g = static_cast<float>(static_cast<uint8_t>(color >> 8)) / 255.0f;
        float const b = static_cast<float>(static_cast<uint8_t>(color >> 16)) / 255.0f;

        float const exp = std::max(gamma, 1.0F);

        return ColorRef{
            .Red = std::pow(r, exp),
            .Green = std::pow(g, exp),
            .Blue = std::pow(b, exp),
            .Alpha = 1.0f,
        };
    }
}

namespace Anemone::System
{
    static ProcessorProperties DetermineProcessorPropertiesOnce()
    {
        ProcessorProperties result{};

        // Determine Cache properties.
        {
            DWORD dwSize{};
            GetLogicalProcessorInformationEx(RelationCache, nullptr, &dwSize);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                std::vector<std::byte> buffer{dwSize};

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

                    cacheLineSize = std::min<uint32_t>(cacheLineSize, current.Cache.LineSize);

                    switch (current.Cache.Level)
                    {
                    case 1:
                        result.CacheSizeLevel1 += current.Cache.CacheSize;
                        break;

                    case 2:
                        result.CacheSizeLevel2 += current.Cache.CacheSize;
                        break;

                    case 3:
                        result.CacheSizeLevel3 += current.Cache.CacheSize;
                        break;

                    case 4:
                        result.CacheSizeLevel4 += current.Cache.CacheSize;
                        break;

                    default:
                        // Unknown cache line level. Are you some kind of server from the future?
                        break;
                    }

                    it += current.Size;
                }

                result.CacheLineSize = cacheLineSize;
            }
        }

        // Determine CPUs
        {
            DWORD dwSize{};
            GetSystemCpuSetInformation(nullptr, 0, &dwSize, GetCurrentProcess(), 0);

            if ((GetLastError() == ERROR_INSUFFICIENT_BUFFER) and (dwSize != 0))
            {
                std::vector<std::byte> buffer{dwSize};

                GetSystemCpuSetInformation(
                    reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.data()),
                    dwSize,
                    &dwSize,
                    GetCurrentProcess(),
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
                                ++result.EfficiencyCores;
                            }
                            else
                            {
                                ++result.PerformanceCores;
                            }

                            ++result.PhysicalCores;
                        }

                        ++result.LogicalCores;
                        break;
                    }

                    it += current.Size;
                }

                if (result.PerformanceCores == 0)
                {
                    // Non-hybrid CPUs report all cores as efficient cores.
                    result.PerformanceCores = result.EfficiencyCores;
                    result.EfficiencyCores = 0;
                }
            }
        }

        // Query CPU name from registry
        {
            Platform::win32_string_buffer<wchar_t, 128> buffer{};

            Platform::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(ProcessorNameString)");
            Platform::win32_NarrowString(result.Name, buffer.as_view());

            Platform::win32_QueryRegistry(buffer, HKEY_LOCAL_MACHINE, LR"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", LR"(VendorIdentifier)");
            Platform::win32_NarrowString(result.Vendor, buffer.as_view());
        }

        return result;
    }

    ProcessorProperties const& GetProcessorProperties()
    {
        static ProcessorProperties const properties = DetermineProcessorPropertiesOnce();
        return properties;
    }
}
