#pragma once
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Base/Duration.hxx"
#include "AnemoneRuntime/Base/DateTime.hxx"
#include "AnemoneRuntime/Base/Instant.hxx"
#include "AnemoneRuntime/Base/Uuid.hxx"

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

    struct ProcessMemoryUsage final
    {
        uint64_t PageFaultCount;
        uint64_t PeakWorkingSetSize;
        uint64_t WorkingSetSize;
        uint64_t QuotaPeakPagedPoolUsage;
        uint64_t QuotaPagedPoolUsage;
        uint64_t QuotaPeakNonPagedPoolUsage;
        uint64_t QuotaNonPagedPoolUsage;
        uint64_t PageFileUsage;
        uint64_t PeakPageFileUsage;        
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

    enum class ProcessorCharacteristics : uint8_t
    {
        Performance,
        Efficiency,
    };

    struct ProcessorTopology
    {
        ProcessorCharacteristics Characteristics;
        uint32_t PackageId;
        uint32_t CoreId;
        uint32_t ThreadId;
        uint32_t CacheLineSize;

        uint32_t CacheL1;
        uint32_t CacheL2;
        uint32_t CacheL3;
    };

}

namespace Anemone
{
    class Environment final
    {
    public:
        RUNTIME_API static auto GetEnvironmentVariable(std::string& result, std::string_view name) -> bool;

        RUNTIME_API static auto SetEnvironmentVariable(std::string name, std::string_view value) -> bool;

        RUNTIME_API static auto RemoveEnvironmentVariable(std::string_view name) -> bool;

        RUNTIME_API static void GetDisplayMetrics(DisplayMetrics& metrics);

        RUNTIME_API static auto GetScreenPixel(Math::PointF position, float gamma) -> ColorRef;

        RUNTIME_API static auto GetSystemVersion() -> std::string_view;

        RUNTIME_API static auto GetSystemId() -> Uuid;

        RUNTIME_API static auto GetSystemName() -> std::string_view;

        RUNTIME_API static auto GetSystemUptime() -> Duration;

        RUNTIME_API static auto GetApplicationStartupTime() -> DateTime;

        RUNTIME_API static auto GetMemoryProperties() -> MemoryProperties;

        RUNTIME_API static auto GetMemoryUsage() -> MemoryUsage;

        RUNTIME_API static auto GetPowerUsage() -> PowerUsage;

        RUNTIME_API static auto GetProcessorUsage() -> ProcessorUsage;

        RUNTIME_API static auto GetProcessMemoryUsage() -> ProcessMemoryUsage;

        RUNTIME_API static void Terminate(bool force);

        RUNTIME_API static bool IsConsoleApplication();

        RUNTIME_API static auto GetDeviceUniqueId() -> std::string_view;

        RUNTIME_API static auto GetDeviceName() -> std::string_view;

        RUNTIME_API static auto GetDeviceModel() -> std::string_view;

        RUNTIME_API static auto GetDeviceType() -> DeviceType;

        RUNTIME_API static auto GetDeviceProperties() -> DeviceProperties;

        RUNTIME_API static auto GetComputerName() -> std::string_view;

        RUNTIME_API static auto GetUserName() -> std::string_view;

        RUNTIME_API static auto GetExecutablePath() -> std::string_view;

        RUNTIME_API static auto GetStartupPath() -> std::string_view;

        RUNTIME_API static auto GetHomePath() -> std::string_view;

        RUNTIME_API static auto GetDesktopPath() -> std::string_view;

        RUNTIME_API static auto GetDocumentsPath() -> std::string_view;

        RUNTIME_API static auto GetDownloadsPath() -> std::string_view;

        RUNTIME_API static auto GetTemporaryPath() -> std::string_view;

        RUNTIME_API static auto GetCurrentDateTime() -> DateTime;

        RUNTIME_API static auto GetCurrentDateTimeUtc() -> DateTime;

        RUNTIME_API static auto GetCurrentTimeZoneBias() -> Duration;

        RUNTIME_API static auto GetCurrentInstant() -> Instant;

        RUNTIME_API static void GetRandom(std::span<std::byte> buffer);

        RUNTIME_API static void LaunchUrl(std::string_view url);

        RUNTIME_API static bool IsOnline();

        RUNTIME_API static auto GetPhysicalCoresCount() -> size_t;

        RUNTIME_API static auto GetLogicalCoresCount() -> size_t;

        RUNTIME_API static auto GetPerformanceCoresCount() -> size_t;

        RUNTIME_API static auto GetEfficiencyCoresCount() -> size_t;

        RUNTIME_API static bool IsHyperThreadingEnabled();

        RUNTIME_API static auto GetCacheLineSize() -> size_t;

        RUNTIME_API static auto GetCacheSizeLevel1() -> size_t;

        RUNTIME_API static auto GetCacheSizeLevel2() -> size_t;

        RUNTIME_API static auto GetCacheSizeLevel3() -> size_t;

        RUNTIME_API static auto GetProcessorName() -> std::string_view;

        RUNTIME_API static auto GetProcessorVendor() -> std::string_view;
    };
}
