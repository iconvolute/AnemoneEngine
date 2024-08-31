#include "AnemoneRuntime/System/Static.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"

#include <sys/utsname.h>
#include <pwd.h>

namespace Anemone::System
{
    UninitializedObject<Environment> GEnvironment;
    UninitializedObject<ProcessorProperties> GProcessorProperties;

    static void InitializeEnvironment(Environment& environment)
    {
        // System version
        {
            if (struct utsname name{}; uname(&name) != -1)
            {
                // TODO: Determine OS version from '/etc/lsb-release'?
                environment.SystemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
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
                    environment.SystemId = buffer.data();
                }

                close(fd);
            }
            else if (int fd = open("/var/lib/dbus/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    environment.SystemId = buffer.data();
                }

                close(fd);
            }
            else
            {
                environment.SystemId = "Unknown";
            }
        }

        // Startup path
        {
            std::array<char, PATH_MAX> procpath{};

            if (getcwd(procpath.data(), procpath.size()) != nullptr)
            {
                environment.StartupPath.assign(procpath.data());
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

            environment.ExecutablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                environment.ComputerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                environment.ComputerName.assign(hostname.data());
            }
            else
            {
                environment.ComputerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                environment.UserName = pw->pw_name;
            }
            else
            {
                environment.UserName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                environment.ProfilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    environment.ProfilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            environment.DesktopPath = environment.ProfilePath;
            Path::Push(environment.DesktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            environment.DocumentsPath = environment.ProfilePath;
            Path::Push(environment.DocumentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            environment.DownloadsPath = environment.ProfilePath;
            Path::Push(environment.DownloadsPath, "Downloads");
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

            environment.TempPath.assign(value);
#endif
        }

        // Startup time
        {
            environment.StartupTime = DateTime::Now();
        }

    }

    static void InitializeProcessorProperties(ProcessorProperties& properties)
    {
        // Number of processors
        {
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
        }
    }

    void InitializeStatic()
    {
        GEnvironment.Create();
        InitializeEnvironment(GEnvironment.Get());

        GProcessorProperties.Create();
        InitializeProcessorProperties(GProcessorProperties.Get());
    }

    void ShutdownStatic()
    {
        GProcessorProperties.Destroy();
        GEnvironment.Destroy();
    }
}
