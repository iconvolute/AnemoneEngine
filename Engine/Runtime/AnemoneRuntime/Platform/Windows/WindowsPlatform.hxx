#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsEnvironment.hxx"

#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

#include <string>
#include <wrl/client.h>
#include <netlistmgr.h>

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

        Microsoft::WRL::ComPtr<INetworkListManager> NetworkListManager{};
    };

    extern UninitializedObject<WindowsPlatformStatics> GWindowsPlatformStatics;
}
