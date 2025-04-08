#pragma once
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Uuid.hxx"

#include <span>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace Anemone
{
    struct MemoryProperties
    {
        uint64_t TotalPhysicalMemory;
        uint64_t TotalVirtualMemory;
        uint64_t TotalPageFile;
        uint64_t AvailablePhysicalMemory;
        uint64_t AvailableVirtualMemory;
        uint64_t AvailablePageFile;
        uint64_t SystemAllocationGranularity;
        uint64_t PageSize;
        uint64_t AddressLimitLow;
        uint64_t AddressLimitHigh;
    };

    enum class PowerState : uint8_t
    {
        Unknown,
        Charging,
        Discharging,
        NoBattery,
        Charged,
    };

    struct PowerUsage final
    {
        PowerState State;
        int32_t BatteryLevel;
    };

    struct MemoryUsage final
    {
        uint64_t AvailableVirtual;
        uint64_t AvailablePhysical;
        uint64_t UsedVirtual;
        uint64_t UsedPhysical;
        uint64_t PeakUsedVirtual;
        uint64_t PeakUsedPhysical;
    };

    struct ProcessorUsage final
    {
        Duration UserTime;
        Duration KernelTime;
    };

    enum class DeviceType : uint8_t
    {
        Unknown,
        Server,
        Desktop,
        Console,
        Handheld,
    };

    struct DeviceProperties
    {
        bool Gyroscope;
        bool LocationService;
        bool Vibration;
        bool Audio;
    };

    // fixme: should this be defined in Math?
    struct ColorRef final
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };

    enum class DisplayOrientation : uint8_t
    {
        Unknown,
        Landscape,
        LandscapeFlipped,
        Portrait,
        PortraitFlipped,
    };

    struct DisplayInfo
    {
        std::string Name;
        std::string Id;
        DisplayOrientation Orientation;
        bool Primary;
        Math::RectF DisplayRect;
        Math::RectF WorkAreaRect;
    };

    struct DisplayMetrics
    {
        std::vector<DisplayInfo> Displays;
        Math::RectF VirtualDisplayRect;
        Math::RectF PrimaryDisplayWorkArea;
        Math::SizeF PrimaryDisplaySize;
    };

    enum class PlatformId : uint32_t
    {
        Unknown = 0,
        Windows = 1,
        Linux = 2,
        Android = 3,
    };
}

namespace Anemone
{
    struct Environment final
    {
        RUNTIME_API static bool GetEnvironmentVariable(std::string& result, std::string_view name);
        RUNTIME_API static bool SetEnvironmentVariable(std::string name, std::string_view value);
        RUNTIME_API static bool RemoveEnvironmentVariable(std::string_view name);

        RUNTIME_API static void GetDisplayMetrics(DisplayMetrics& metrics);
        RUNTIME_API static ColorRef GetScreenPixel(Math::PointF position, float gamma);

        RUNTIME_API static std::string_view GetSystemVersion();
        RUNTIME_API static Uuid GetSystemId();
        RUNTIME_API static std::string_view GetSystemName();

        RUNTIME_API static Duration GetSystemUptime();
        RUNTIME_API static DateTime GetApplicationStartupTime();
        RUNTIME_API static MemoryProperties GetMemoryProperties();
        RUNTIME_API static MemoryUsage GetMemoryUsage();
        RUNTIME_API static PowerUsage GetPowerUsage();
        RUNTIME_API static ProcessorUsage GetProcessorUsage();
        RUNTIME_API static void Terminate(bool force);
        RUNTIME_API static std::string_view GetDeviceUniqueId();
        RUNTIME_API static std::string_view GetDeviceName();
        RUNTIME_API static std::string GetDeviceModel();
        RUNTIME_API static DeviceType GetDeviceType();
        RUNTIME_API static DeviceProperties GetDeviceProperties();

        RUNTIME_API static std::string_view GetComputerName();
        RUNTIME_API static std::string_view GetUserName();

        RUNTIME_API static std::string_view GetExecutablePath();
        RUNTIME_API static std::string_view GetStartupPath();
        RUNTIME_API static std::string_view GetHomePath();
        RUNTIME_API static std::string_view GetDesktopPath();
        RUNTIME_API static std::string_view GetDocumentsPath();
        RUNTIME_API static std::string_view GetDownloadsPath();
        RUNTIME_API static std::string_view GetTemporaryPath();

        RUNTIME_API static DateTime GetCurrentDateTime();
        RUNTIME_API static DateTime GetCurrentDateTimeUtc();
        RUNTIME_API static Duration GetCurrentTimeZoneBias();
        RUNTIME_API static Instant GetCurrentInstant();

        RUNTIME_API static void GetRandom(std::span<std::byte> buffer);

        RUNTIME_API static void LaunchUrl(std::string_view url);

        RUNTIME_API static bool IsOnline();
    };
}
