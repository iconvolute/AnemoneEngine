#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"

#include <sys/utsname.h>
#include <pwd.h>

namespace Anemone::System
{
    bool GetEnvironmentVariable(std::string& result, std::string_view name)
    {
        // TODO: promote win32_string_buffer and use it here instead of this `std::string`
#if ANEMONE_PLATFORM_ANDROID
        char const* variable = getenv(std::string{name}.c_str());
#else
        char const* variable = secure_getenv(std::string{name}.c_str());
#endif

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
}

namespace Anemone::System
{
    bool GetClipboardContent(std::string& value)
    {
        (void)value;
        return false;
    }

    bool SetClipboardContent(std::string_view value)
    {
        (void)value;
        return false;
    }
}

namespace Anemone::System
{
    void GetDisplayMetrics(DisplayMetrics& displayMetrics)
    {
        displayMetrics.Displays.clear();
    }
}

namespace Anemone::System
{
    ColorRef GetScreenPixel(Point<int32_t> position, float gamma)
    {
        (void)position;
        (void)gamma;
        return ColorRef{};
    }
}

namespace Anemone::System
{
    static ProcessorProperties GProcessorProperties = []() -> ProcessorProperties
    {
        ProcessorProperties result{};

        // Number of processors
        {
            // This may not be accurate on all systems.
            result.PhysicalCores = sysconf(_SC_NPROCESSORS_CONF);

            cpu_set_t mask;
            CPU_ZERO(&mask);

            if (sched_getaffinity(0, sizeof(mask), &mask) == 0)
            {
                result.LogicalCores = CPU_COUNT(&mask);
            }
            else
            {
                result.LogicalCores = result.PhysicalCores;
            }

            if (result.PerformanceCores == 0)
            {
                result.PerformanceCores = result.PhysicalCores;
                result.EfficiencyCores = 0;
            }
        }

        return result;
    }();

    ProcessorProperties const& GetProcessorProperties()
    {
        return GProcessorProperties;
    }
}

namespace Anemone::System
{
    static Environment GEnvironment = []() -> Environment
    {
        Environment result{};

        // System version
        {
            if (struct utsname name{}; uname(&name) != -1)
            {
                // TODO: Determine OS version from '/etc/lsb-release'?
                result.SystemVersion = fmt::format("{}-{}-{}", name.sysname, name.release, name.version);
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
                    result.SystemId = buffer.data();
                }

                close(fd);
            }
            else if (int fd = open("/var/lib/dbus/machine-id", O_RDONLY); fd != -1)
            {
                ssize_t processed = read(fd, buffer.data(), buffer.size() - 1);

                if (processed > 0)
                {
                    buffer[32] = 0;
                    result.SystemId = buffer.data();
                }

                close(fd);
            }
            else
            {
                result.SystemId = "Unknown";
            }
        }

        // Startup path
        {
            std::array<char, PATH_MAX> procpath{};

            if (getcwd(procpath.data(), procpath.size()) != nullptr)
            {
                result.StartupPath.assign(procpath.data());
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

            result.ExecutablePath.assign(execpath.data(), static_cast<size_t>(length));
        }

        // Computer name
        {
            std::array<char, HOST_NAME_MAX> hostname{};
            struct utsname name = {};

            if (uname(&name) == 0)
            {
                result.ComputerName.assign(name.nodename);
            }
            else if (gethostname(hostname.data(), hostname.size()) == 0)
            {
                result.ComputerName.assign(hostname.data());
            }
            else
            {
                result.ComputerName.assign("Linux Machine");
            }
        }

        // User name
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw != nullptr)
            {
                result.UserName = pw->pw_name;
            }
            else
            {
                result.UserName = "Unknown";
            }
        }

        // Profile path
        {
#if !ANEMONE_PLATFORM_ANDROID
            const char* value = secure_getenv("HOME");

            if (value != nullptr)
            {
                result.ProfilePath.assign(value);
            }
            else
            {
                struct passwd* userinfo = getpwuid(geteuid());

                if (userinfo != nullptr && userinfo->pw_dir != nullptr)
                {
                    result.ProfilePath.assign(userinfo->pw_dir);
                }
            }
#endif
        }

        // UserDesktopPath
        {
            result.DesktopPath = result.ProfilePath;
            Path::Push(result.DesktopPath, "Desktop");
        }

        // UserDocumentsPath
        {
            result.DocumentsPath = result.ProfilePath;
            Path::Push(result.DocumentsPath, "Documents");
        }

        // UserDownloadsPath
        {
            result.DownloadsPath = result.ProfilePath;
            Path::Push(result.DownloadsPath, "Downloads");
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

            result.TempPath.assign(value);
#endif
        }

        // Startup time
        {
            result.StartupTime = DateTime::Now();
        }

        return result;
    }();

    Environment const& GetEnvironment()
    {
        return GEnvironment;
    }
}
