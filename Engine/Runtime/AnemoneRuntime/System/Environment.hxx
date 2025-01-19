#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"
#include "AnemoneRuntime/DateTime.hxx"

#include <string_view>
#include <string>
#include <vector>

namespace Anemone
{
    struct ISubsystem
    {
        virtual ~ISubsystem() = 0;
        virtual bool IsRunning() const = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
    };

    struct ISubsystemDescriptor
    {
        virtual ~ISubsystemDescriptor() = 0;
        virtual std::string_view GetName() const = 0;
        virtual ISubsystem* Create() const = 0;
    };
}

namespace Anemone::System
{
    struct ProcessorProperties final
    {
        uint32_t PhysicalCores{};
        uint32_t LogicalCores{};
        uint32_t PerformanceCores{};
        uint32_t EfficiencyCores{};

        uint32_t CacheSizeLevel1{};
        uint32_t CacheSizeLevel2{};
        uint32_t CacheSizeLevel3{};
        uint32_t CacheSizeLevel4{};

        uint32_t CacheLineSize{};

        std::string Name; // NOTE: Do we really need this?
        std::string Vendor;
    };

    RUNTIME_API ProcessorProperties const& GetProcessorProperties();

    struct Environment final
    {
        std::string SystemVersion;
        std::string SystemId;

        std::string ExecutablePath;
        std::string StartupPath;

        std::string ComputerName;
        std::string UserName;

        std::string ProfilePath;
        std::string DesktopPath;
        std::string DocumentsPath;
        std::string DownloadsPath;
        std::string TempPath;

        DateTime StartupTime;
    };

    RUNTIME_API Environment const& GetEnvironment();
}
