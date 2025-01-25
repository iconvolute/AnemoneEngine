#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BasePlatform.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxEnvironment.hxx"

#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string>

// *** NOTE ***
// This file should be included only in the Windows platform implementation files.

namespace Anemone::Internal
{
    struct LinuxPlatformStatics final
    {
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
    };

    extern UninitializedObject<LinuxPlatformStatics> GLinuxPlatformStatics;
}

namespace Anemone
{
    struct LinuxPlatform final
    {
        static RUNTIME_API void Initialize();
        static RUNTIME_API void Finalize();
    };

    using Platform = LinuxPlatform;
}
