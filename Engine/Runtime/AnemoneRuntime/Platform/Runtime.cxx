#include "AnemoneRuntime/Platform/Runtime.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

#if ANEMONE_BUILD_PROFILING
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Profiler/ProfilerNvidia.hxx"
#endif


#include <fmt/format.h>

namespace Anemone::Platform
{
    void InitializeRuntime_PlatformSpecific();
    void FinalizeRuntime_PlatformSpecific();

    void InitializeRuntime(int argc, char** argv)
    {
        (void)argc;
        (void)argv;

        InitializeRuntime_PlatformSpecific();

        auto const& processor = System::GetProcessorProperties();

        fmt::println("PhysicalCores:    '{}'", processor.PhysicalCores);
        fmt::println("LogicalCores:     '{}'", processor.LogicalCores);
        fmt::println("PerformanceCores: '{}'", processor.PerformanceCores);
        fmt::println("EfficiencyCores:  '{}'", processor.EfficiencyCores);

        fmt::println("CacheSizeLevel1:  '{}'", processor.CacheSizeLevel1);
        fmt::println("CacheSizeLevel2:  '{}'", processor.CacheSizeLevel2);
        fmt::println("CacheSizeLevel3:  '{}'", processor.CacheSizeLevel3);
        fmt::println("CacheSizeLevel4:  '{}'", processor.CacheSizeLevel4);

        fmt::println("CacheLineSize:    '{}'", processor.CacheLineSize);

        fmt::println("Name:             '{}'", processor.Name);
        fmt::println("Vendor:           '{}'", processor.Vendor);

        auto const& environment = System::GetEnvironment();
        fmt::println("SystemVersionString: '{}'", environment.SystemVersion);
        fmt::println("SystemId:            '{}'", environment.SystemId);
        fmt::println("ExecutablePath:      '{}'", environment.ExecutablePath);
        fmt::println("StartupPath:         '{}'", environment.StartupPath);
        fmt::println("ComputerName:        '{}'", environment.ComputerName);
        fmt::println("UserName:            '{}'", environment.UserName);
        fmt::println("DesktopPath:         '{}'", environment.DesktopPath);
        fmt::println("DocumentsPath:       '{}'", environment.DocumentsPath);
        fmt::println("ProfilePath:         '{}'", environment.ProfilePath);
        fmt::println("TempPath:            '{}'", environment.TempPath);
        fmt::println("StartupTime:         '{}'", environment.StartupTime);

        // Initialize profiler
#if ANEMONE_BUILD_PROFILING
        Anemone::Profiler::GProfiler = new Anemone::Profiler::ProfilerNvidia();
#endif
    }

    void FinalizeRuntime()
    {
        FinalizeRuntime_PlatformSpecific();
    }
}
