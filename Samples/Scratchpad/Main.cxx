#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Initialize.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

#include <fmt/format.h>
#include <atomic>

AE_DECLARE_PROFILE(Main);
AE_DECLARE_PROFILE(Outer);
AE_DECLARE_PROFILE(Inner);

int main(int argc, char* argv[])
{
    Anemone::RuntimeInitializer runtime{argc, argv};

    auto const& processor = Anemone::System::GetProcessorProperties();

    AE_LOG(Info, "PhysicalCores:       '{}'\n", processor.PhysicalCores);
    AE_LOG(Info, "LogicalCores:        '{}'\n", processor.LogicalCores);
    AE_LOG(Info, "PerformanceCores:    '{}'\n", processor.PerformanceCores);
    AE_LOG(Info, "EfficiencyCores:     '{}'\n", processor.EfficiencyCores);
    AE_LOG(Info, "CacheSizeLevel1:     '{}'\n", processor.CacheSizeLevel1);
    AE_LOG(Info, "CacheSizeLevel2:     '{}'\n", processor.CacheSizeLevel2);
    AE_LOG(Info, "CacheSizeLevel3:     '{}'\n", processor.CacheSizeLevel3);
    AE_LOG(Info, "CacheSizeLevel4:     '{}'\n", processor.CacheSizeLevel4);
    AE_LOG(Info, "CacheLineSize:       '{}'\n", processor.CacheLineSize);
    AE_LOG(Info, "Name:                '{}'\n", processor.Name);
    AE_LOG(Info, "Vendor:              '{}'\n", processor.Vendor);

    auto const& environment = Anemone::System::GetEnvironment();
    AE_LOG(Info, "SystemVersionString: '{}'\n", environment.SystemVersion);
    AE_LOG(Info, "SystemId:            '{}'\n", environment.SystemId);
    AE_LOG(Info, "ExecutablePath:      '{}'\n", environment.ExecutablePath);
    AE_LOG(Info, "StartupPath:         '{}'\n", environment.StartupPath);
    AE_LOG(Info, "ComputerName:        '{}'\n", environment.ComputerName);
    AE_LOG(Info, "UserName:            '{}'\n", environment.UserName);
    AE_LOG(Info, "DesktopPath:         '{}'\n", environment.DesktopPath);
    AE_LOG(Info, "DocumentsPath:       '{}'\n", environment.DocumentsPath);
    AE_LOG(Info, "ProfilePath:         '{}'\n", environment.ProfilePath);
    AE_LOG(Info, "TempPath:            '{}'\n", environment.TempPath);
    AE_LOG(Info, "StartupTime:         '{}'\n", environment.StartupTime);

    {
        AE_PROFILE_SCOPE(Main);
        {
            AE_PROFILE_SCOPE(Inner);
            AE_LOG(Warning, "Hello {}\n", Anemone::System::GetEnvironment().StartupTime);
            fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
            Anemone::Diagnostic::GetCurrentStackTrace([&](auto const& entry)
            {
                AE_LOG(Warning, "{}\n", entry);
                return true;
            });
        }

        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        {
            AE_PROFILE_SCOPE(Outer);
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
            {
                AE_PROFILE_SCOPE(Inner);
                Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));

                AE_LOG(Fatal, "Crashed!?\n");
                //std::atomic<int>* p = nullptr;
                //p->store(42);
            }
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        }
        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
    }

    return 0;
}
