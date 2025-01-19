#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

#include <string>
#include <string_view>
#include <vector>

namespace Anemone::Platform
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

    struct IPlatformEvents;

    class Window;
}

namespace Anemone::Platform::Internal
{
    struct PlatformStatics final
    {
        IPlatformEvents* EventHandler{};

        ProcessorProperties Processor;

        std::string SystemVersion;
        std::string SystemId;
        DateTime ApplicationStartupTime;

        std::string ComputerName;
        std::string UserName;

        std::string ExecutablePath;
        std::string StartupPath;

        std::string ProfilePath;
        std::string DesktopPath;
        std::string DocumentsPath;
        std::string DownloadsPath;
        std::string TemporaryPath;

        IntrusiveList<Window> WindowsCollection{};
    };

    RUNTIME_API extern UninitializedObject<PlatformStatics> GPlatformStatics;

    RUNTIME_API void Initialize(int argc, char* argv[]);
    RUNTIME_API void Finalize();
}

namespace Anemone::Platform
{
    // Clipboard
    RUNTIME_API bool GetClipboardText(std::string& result);

    RUNTIME_API bool SetClipboardText(std::string_view value);

    // Environment
    RUNTIME_API bool GetEnvironmentVariable(std::string& result, std::string_view name);

    RUNTIME_API bool SetEnvironmentVariable(std::string_view name, std::string_view value);

    RUNTIME_API bool RemoveEnvironmentVariable(std::string_view name);

    // Processor
    inline ProcessorProperties GetProcessorProperties()
    {
        return Internal::GPlatformStatics->Processor;
    }

    RUNTIME_API std::vector<ProcessorTopology> GetProcessorTopology();

    // Display
    RUNTIME_API void GetDisplayMetrics(DisplayMetrics& displayMetrics);

    RUNTIME_API ColorRef GetScreenPixel(Math::PointF position, float gamma);

    // System
    inline std::string_view GetSystemVersion()
    {
        return Internal::GPlatformStatics->SystemVersion;
    }

    inline std::string_view GetSystemId()
    {
        return Internal::GPlatformStatics->SystemId;
    }

    RUNTIME_API std::string_view GetOperatingSystem();

    RUNTIME_API Duration GetSystemUptime();

    inline DateTime GetApplicationStartupTime()
    {
        return Internal::GPlatformStatics->ApplicationStartupTime;
    }

    RUNTIME_API MemoryProperties GetMemoryProperties();

    RUNTIME_API MemoryUsage GetMemoryUsage();

    RUNTIME_API PowerUsage GetPowerUsage();

    RUNTIME_API ProcessorUsage GetProcessorUsage();

    RUNTIME_API void Terminate(bool force);

    // Device
    RUNTIME_API std::string_view GetDeviceUniqueId();

    RUNTIME_API std::string_view GetDeviceName();

    RUNTIME_API std::string_view GetDeviceModel();

    RUNTIME_API DeviceType GetDeviceType();

    RUNTIME_API DeviceProperties GetDeviceProperties();

    inline std::string_view GetComputerName()
    {
        return Internal::GPlatformStatics->ComputerName;
    }

    inline std::string_view GetUserName()
    {
        return Internal::GPlatformStatics->UserName;
    }

    // Paths
    inline std::string_view GetExecutablePath()
    {
        return Internal::GPlatformStatics->ExecutablePath;
    }

    inline std::string_view GetStartupPath()
    {
        return Internal::GPlatformStatics->StartupPath;
    }

    inline std::string_view GetHomePath()
    {
        return Internal::GPlatformStatics->ProfilePath;
    }

    inline std::string_view GetDesktopPath()
    {
        return Internal::GPlatformStatics->DesktopPath;
    }

    inline std::string_view GetDocumentsPath()
    {
        return Internal::GPlatformStatics->DocumentsPath;
    }

    inline std::string_view GetDownloadsPath()
    {
        return Internal::GPlatformStatics->DownloadsPath;
    }

    inline std::string_view GetTemporaryPath()
    {
        return Internal::GPlatformStatics->TemporaryPath;
    }

    // Events
    inline void SetPlatformEvents(IPlatformEvents* events)
    {
        Internal::GPlatformStatics->EventHandler = events;
    }

    // Error Reporting
    RUNTIME_API AssertAction HandleAssertion(
        std::source_location const& location,
        std::string_view expression,
        std::string_view message);

    RUNTIME_API void HandlePanic();

    // Diagnostics
    RUNTIME_API void DebugBreak();

    [[noreturn]] RUNTIME_API void Crash();

    RUNTIME_API bool IsDebuggerAttached();

    RUNTIME_API void WaitForDebugger();

    RUNTIME_API bool AttachDebugger();

    // Date/Time
    RUNTIME_API DateTime GetCurrentDateTime();

    RUNTIME_API DateTime GetCurrentDateTimeUtc();

    RUNTIME_API Duration GetCurrentTimeZoneBias();

    // OS random number generator
    RUNTIME_API void GetRandom(std::span<std::byte> buffer);

    RUNTIME_API void ProcessMessages();
}

namespace Anemone
{
    struct ITargetPlatform
    {
    };
}
