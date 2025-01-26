#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BasePlatform.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsEnvironment.hxx"

#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string>

// *** NOTE ***
// This file should be included only in the Windows platform implementation files.

namespace Anemone::Internal
{
    struct WindowsPlatformStatics final
    {
        HINSTANCE InstanceHandle;

        ProcessorProperties Processor{};

        DateTime ApplicationStartupTime{};

        std::string SystemVersion{};
        std::string SystemId{};

        std::string ComputerName{};
        std::string UserName{};

        std::string ExecutablePath{};
        std::string StartupPath{};
        std::string ProfilePath{};
        std::string DesktopPath{};
        std::string DocumentsPath{};
        std::string DownloadsPath{};
        std::string TemporaryPath{};
        std::string DeviceId{};
        std::string DeviceManufacturer{};
        std::string DeviceName{};
        std::string DeviceVersion{};
    };

    extern UninitializedObject<WindowsPlatformStatics> GWindowsPlatformStatics;
}

namespace Anemone
{
    struct WindowsPlatform final
    {
        static RUNTIME_API void Initialize();
        static RUNTIME_API void Finalize();
    };

    using Platform = WindowsPlatform;
}
