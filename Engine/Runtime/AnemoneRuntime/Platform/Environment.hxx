#pragma once
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"

#include <span>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace Anemone
{
    // Simple CPU properties.
    struct ProcessorProperties
    {
        // Number of physical cores.
        uint32_t PhysicalCores;

        // Number of logical cores.
        uint32_t LogicalCores;

        // Number of performance cores.
        uint32_t PerformanceCores;

        // Number of efficiency cores.
        uint32_t EfficiencyCores;

        bool HyperThreadingEnabled;

        // Smallest cache-line size found.
        uint32_t CacheLineSize;

        uint32_t CacheSizeLevel1;
        uint32_t CacheSizeLevel2;
        uint32_t CacheSizeLevel3;
        std::string Name;
        std::string Vendor;
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
        static RUNTIME_API bool GetEnvironmentVariable(std::string& result, std::string_view name);
        static RUNTIME_API bool SetEnvironmentVariable(std::string name, std::string_view value);
        static RUNTIME_API bool RemoveEnvironmentVariable(std::string_view name);

        static RUNTIME_API ProcessorProperties const& GetProcessorProperties();
        static RUNTIME_API std::vector<ProcessorTopology> GetProcessorTopology();

        static RUNTIME_API void GetDisplayMetrics(DisplayMetrics& metrics);
        static RUNTIME_API ColorRef GetScreenPixel(Math::PointF position, float gamma);

        static RUNTIME_API std::string_view GetSystemVersion();
        static RUNTIME_API std::string_view GetSystemId();
        static RUNTIME_API std::string_view GetSystemName();

        static RUNTIME_API Duration GetSystemUptime();
        static RUNTIME_API DateTime GetApplicationStartupTime();
        static RUNTIME_API MemoryProperties GetMemoryProperties();
        static RUNTIME_API MemoryUsage GetMemoryUsage();
        static RUNTIME_API PowerUsage GetPowerUsage();
        static RUNTIME_API ProcessorUsage GetProcessorUsage();
        static RUNTIME_API void Terminate(bool force);
        static RUNTIME_API std::string_view GetDeviceUniqueId();
        static RUNTIME_API std::string_view GetDeviceName();
        static RUNTIME_API std::string GetDeviceModel();
        static RUNTIME_API DeviceType GetDeviceType();
        static RUNTIME_API DeviceProperties GetDeviceProperties();

        static RUNTIME_API std::string_view GetComputerName();
        static RUNTIME_API std::string_view GetUserName();

        static RUNTIME_API std::string_view GetExecutablePath();
        static RUNTIME_API std::string_view GetStartupPath();
        static RUNTIME_API std::string_view GetHomePath();
        static RUNTIME_API std::string_view GetDesktopPath();
        static RUNTIME_API std::string_view GetDocumentsPath();
        static RUNTIME_API std::string_view GetDownloadsPath();
        static RUNTIME_API std::string_view GetTemporaryPath();

        static RUNTIME_API DateTime GetCurrentDateTime();
        static RUNTIME_API DateTime GetCurrentDateTimeUtc();
        static RUNTIME_API Duration GetCurrentTimeZoneBias();

        static void GetRandom(std::span<std::byte> buffer);
    };
}
