#include "AnemoneRuntime/Platform/Platform.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxPlatform.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"

#include "AnemoneRuntime/System/Path.hxx"

#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/auxv.h>
#include <sys/random.h>
#include <pwd.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sched.h>

#if ANEMONE_ARCHITECTURE_X64
#include <cpuid.h>
#elif ANEMONE_ARCHITECTURE_ARM64
#include <asm/hwcap.h>
#endif

#include <clocale>

namespace Anemone::Internal
{
    UninitializedObject<LinuxPlatformStatics> GLinuxPlatformStatics;

    namespace
    {
        void VerifyRequirements()
        {
#if ANEMONE_ARCHITECTURE_X64

            int level0[4]{-1};
            __cpuid(0, level0[0], level0[1], level0[2], level0[3]);

            if (level0[0] < 1)
            {
                Debugger::ReportApplicationStop("Application stop: CPU Features not available\n");
            }

            int level1[4]{-1};
            __cpuid(1, level1[0], level1[1], level1[2], level1[3]);

            if (!(level1[2] & bit_CMPXCHG16B))
            {
                Debugger::ReportApplicationStop("Application stop: CMPXCHG16B is not supported\n");
            }

            if (!(level1[2] & bit_POPCNT))
            {
                Debugger::ReportApplicationStop("Application stop: POPCNT is not supported\n");
            }

#if ANEMONE_FEATURE_AVX
            if (!(level1[2] & bit_OSXSAVE))
            {
                Debugger::ReportApplicationStop("Application stop: OSXSAVE is not supported\n");
            }

            if (!(level1[2] & bit_SSE3))
            {
                Debugger::ReportApplicationStop("Application stop: SSE3 is not supported\n");
            }

            if (!(level1[2] & bit_SSE4_1))
            {
                Debugger::ReportApplicationStop("Application stop: SSE4_1 is not supported\n");
            }

            if (!(level1[2] & bit_AVX))
            {
                Debugger::ReportApplicationStop("Application stop: AVX is not supported\n");
            }

#if ANEMONE_FEATURE_AVX2

            if (!(level1[2] & bit_F16C))
            {
                Debugger::ReportApplicationStop("Application stop: F16C is not supported\n");
            }

            if (!(level1[2] & bit_FMA))
            {
                Debugger::ReportApplicationStop("Application stop: FMA is not supported\n");
            }

            int level7_0[4]{-1};
            __cpuid_count(7, 0, level7_0[0], level7_0[1], level7_0[2], level7_0[3]);

            if (!(level7_0[1] & bit_AVX2))
            {
                Debugger::ReportApplicationStop("Application stop: AVX2 is not supported\n");
            }

#endif

#endif

#endif


#if ANEMONE_ARCHITECTURE_ARM64

            auto const cap = getauxval(AT_HWCAP);

            if ((cap & HWCAP_FP) != HWCAP_FP)
            {
                Debugger::ReportApplicationStop("Application stop: NEON not supported\n");
            }

            if ((cap & HWCAP_ASIMD) != HWCAP_ASIMD)
            {
                Debugger::ReportApplicationStop("Application stop: ASIMD not supported\n");
            }

            if ((cap & HWCAP_ATOMICS) != HWCAP_ATOMICS)
            {
                Debugger::ReportApplicationStop("Application stop: ATOMICS not supported\n");
            }

#endif
        }

        ProcessorProperties DetermineProcessorProperties()
        {
            ProcessorProperties properties{};

            // This may not be accurate on all systems.
            properties.PhysicalCores = sysconf(_SC_NPROCESSORS_CONF);

            cpu_set_t mask;
            CPU_ZERO(&mask);

            if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
            {
                properties.LogicalCores = CPU_COUNT(&mask);
            }
            else
            {
                properties.LogicalCores = properties.PhysicalCores;
            }

            if (properties.PerformanceCores == 0)
            {
                properties.PerformanceCores = properties.PhysicalCores;
                properties.EfficiencyCores = 0;
            }

            return properties;
        }
    }
}

namespace Anemone
{
    void Platform::Initialize()
    {
        Internal::VerifyRequirements();

        Internal::GLinuxPlatformStatics.Create();
        Internal::GLinuxPlatformStatics->ApplicationStartupTime = Environment::GetCurrentDateTime();
        Internal::GLinuxPlatformStatics->Processor = Internal::DetermineProcessorProperties();

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

        // System version
        {
            if (struct utsname name{}; uname(&name) != -1)
            {
                // TODO: Determine OS version from '/etc/lsb-release'?
                Internal::GLinuxPlatformStatics->SystemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
            }
        }

        // System ID
        {
            std::array<char, 128> buffer{};

            if (int fd = open("/etc/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    Internal::GLinuxPlatformStatics->SystemId = buffer.data();
                }

                close(fd);
            }
            else if (int fd = open("/var/lib/dbus/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    Internal::GLinuxPlatformStatics->SystemId = buffer.data();
                }

                close(fd);
            }
            else
            {
                Internal::GLinuxPlatformStatics->SystemId = "Unknown";
            }
        }

        // Startup path
        {
            std::array<char, PATH_MAX> procpath{};

            if (getcwd(procpath.data(), procpath.size()) != nullptr)
            {
                Internal::GLinuxPlatformStatics->StartupPath.assign(procpath.data());
            }
        }

        // Executable
        {
            std::array<char, PATH_MAX> procpath{};
            std::array<char, PATH_MAX> execpath{};

            pid_t pid = getpid();
            snprintf(std::data(procpath), std::size(procpath), "/proc/%d/exe", pid);
            ssize_t length = readlink(procpath.data(), execpath.data(), execpath.size());
            AE_ASSERT(length >= 0);
            length = std::max<ssize_t>(0, length);

            Internal::GLinuxPlatformStatics->ExecutablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                Internal::GLinuxPlatformStatics->ComputerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                Internal::GLinuxPlatformStatics->ComputerName.assign(hostname.data());
            }
            else
            {
                Internal::GLinuxPlatformStatics->ComputerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                Internal::GLinuxPlatformStatics->UserName = pw->pw_name;
            }
            else
            {
                Internal::GLinuxPlatformStatics->UserName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                Internal::GLinuxPlatformStatics->ProfilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    Internal::GLinuxPlatformStatics->ProfilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            Internal::GLinuxPlatformStatics->DesktopPath = Internal::GLinuxPlatformStatics->ProfilePath;
            System::Path::Push(Internal::GLinuxPlatformStatics->DesktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            Internal::GLinuxPlatformStatics->DocumentsPath = Internal::GLinuxPlatformStatics->ProfilePath;
            System::Path::Push(Internal::GLinuxPlatformStatics->DocumentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            Internal::GLinuxPlatformStatics->DownloadsPath = Internal::GLinuxPlatformStatics->ProfilePath;
            System::Path::Push(Internal::GLinuxPlatformStatics->DownloadsPath, "Downloads");
        }

        // Temp path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("TMPDIR");

            if (value == nullptr)
            {
                value = secure_getenv("TMP");
            }

            if (value == nullptr)
            {
                value = secure_getenv("TEMP");
            }

            if (value == nullptr)
            {
                value = secure_getenv("TEMPDIR");
            }

            if (value == nullptr)
            {
                value = "/tmp/";
            }

            Internal::GLinuxPlatformStatics->TemporaryPath.assign(value);
#endif
        }

        // Startup time
        {
            Internal::GLinuxPlatformStatics->ApplicationStartupTime = DateTime::Now();
        }
    }

    void Platform::Finalize()
    {
        Internal::GLinuxPlatformStatics.Destroy();
    }
}
