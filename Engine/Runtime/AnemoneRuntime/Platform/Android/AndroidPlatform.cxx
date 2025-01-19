#include "AnemoneRuntime/Platform/Android/AndroidPlatform.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <sys/resource.h>
//#include <sys/utsname.h>
//#include <sys/sysinfo.h>
//#include <sys/types.h>
//#include <sys/auxv.h>
//#include <pwd.h>
//#include <ifaddrs.h>
//#include <sys/ioctl.h>
//#include <net/if.h>
//#include <net/if_arp.h>
//#include <sched.h>

#include <clocale>

namespace Anemone::Platform::Internal
{
    UninitializedObject<AndroidPlatformStatics> GAndroidPlatformStatics;
    UninitializedObject<PlatformStatics> GPlatformStatics;
}

namespace Anemone::Platform::Internal
{
    void Initialize(int argc, char* argv[])
    {
        (void)argc;
        (void)argv;

        GPlatformStatics.Create();
        GAndroidPlatformStatics.Create();

        // Set locale.
        (void)std::setlocale(LC_ALL, "en_US.UTF-8"); // NOLINT(concurrency-mt-unsafe); this is invoked in main thread.

        // Bump up the file descriptor limit
        if (rlimit64 rl; getrlimit64(RLIMIT_NOFILE, &rl) == 0)
        {
            if ((rl.rlim_cur != RLIM_INFINITY) and (rl.rlim_cur < 8192))
            {
                rl.rlim_cur = 8192;
                setrlimit64(RLIMIT_NOFILE, &rl);
            }
        }
    }

    void Finalize()
    {
        GAndroidPlatformStatics.Destroy();
        GPlatformStatics.Destroy();
    }
}

namespace Anemone::Platform
{
    bool GetClipboardText(std::string& result)
    {
        result = {};
        return false;
    }

    bool SetClipboardText(std::string_view value)
    {
        (void)value;
        return false;
    }

    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        char const* variable = getenv(std::string{name}.c_str());

        if (variable != nullptr)
        {
            result.assign(variable);
        }

        return variable != nullptr;
    }

    bool SetEnvironmentVariable(std::string_view name, std::string_view value)
    {
        return setenv(std::string{name}.c_str(), std::string{value}.c_str(), 1) == 0;
    }

    bool RemoveEnvironmentVariable(std::string_view name)
    {
        return unsetenv(std::string{name}.c_str()) == 0;
    }

    std::vector<ProcessorTopology> GetProcessorTopology()
    {
        return {};
    }

    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics = {};
    }

    ColorRef GetScreenPixel(Math::PointF position, float gamma)
    {
        (void)position;
        (void)gamma;
        return {};
    }

    std::string_view GetOperatingSystem()
    {
        return {};
    }

    Duration GetSystemUptime()
    {
        return {};
    }

    MemoryProperties GetMemoryProperties()
    {
        return {};
    }

    MemoryUsage GetMemoryUsage()
    {
        return {};
    }

    PowerUsage GetPowerUsage()
    {
        return {};
    }

    ProcessorUsage GetProcessorUsage()
    {
        return {};
    }

    void Terminate(bool force)
    {
        (void)force;
        abort();
    }

    std::string_view GetDeviceUniqueId()
    {
        return {};
    }

    std::string_view GetDeviceName()
    {
        return {};
    }

    std::string_view GetDeviceModel()
    {
        return {};
    }

    DeviceType GetDeviceType()
    {
        return {};
    }

    DeviceProperties GetDeviceProperties()
    {
        return {};
    }

    AssertAction HandleAssertion(std::source_location const& location, std::string_view expression, std::string_view message)
    {
        (void)location;
        (void)expression;
        (void)message;
        return AssertAction::Abort;
    }

    void HandlePanic()
    {
    }

    // Diagnostics
    void DebugBreak()
    {
        anemone_debugbreak();
    }

    void Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool IsDebuggerAttached()
    {
        return false;
    }

    void WaitForDebugger()
    {
    }

    bool AttachDebugger()
    {
        return false;
    }

    DateTime GetCurrentDateTime()
    {
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        int64_t const bias = DateTimeOffset::GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + Interop::posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            }};
    }

    DateTime GetCurrentDateTimeUtc()
    {
        struct timespec ts;

        // Get local time
        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        AE_ASSERT(result == 0);

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + Interop::posix_DateAdjustOffset,
                .Nanoseconds = ts.tv_nsec,
            },
        };
    }

    Duration GetCurrentTimeZoneBias()
    {
        time_t seconds = 0;
        tm tmGMT{};
        tm tmLocal{};
        gmtime_r(&seconds, &tmGMT);
        localtime_r(&seconds, &tmLocal);

        return Duration{
            .Seconds = mktime(&tmGMT) - mktime(&tmLocal),
            .Nanoseconds = 0,
        };
    }

    void GetRandom(std::span<std::byte> buffer)
    {
        (void)buffer;
    }

    void ProcessMessages()
    {
    }
}
