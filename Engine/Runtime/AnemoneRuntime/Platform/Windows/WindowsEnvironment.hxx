#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseEnvironment.hxx"

namespace Anemone
{
    struct WindowsEnvironment final
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

        static RUNTIME_API void GetRandom(std::span<std::byte> buffer);

        // Returns the value indicating if the application is a console application.
        static RUNTIME_API bool IsConsoleApplication();

        // Returns the value indicating if console output is redirected.
        static RUNTIME_API bool IsConsoleRedirecting();

        static RUNTIME_API bool IsOnline();
    };

    using Environment = WindowsEnvironment;
}
