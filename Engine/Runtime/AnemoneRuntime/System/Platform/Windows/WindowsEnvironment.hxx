#pragma once
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct WindowsEnvironmentStatics final
    {
        WindowsEnvironmentStatics();
        ~WindowsEnvironmentStatics();

        static void VerifyRequirements();
        
    public:
        DateTime m_StartupTime;

        std::string m_SystemVersion;
        Uuid m_SystemId;
        std::string m_SystemName;
        std::string m_DeviceId;
        std::string m_DeviceName;
        std::string m_DeviceModel;
        std::string m_DeviceManufacturer;
        std::string m_DeviceVersion;
        DeviceType m_DeviceType;
        DeviceProperties m_DeviceProperties;
        std::string m_ComputerName;
        std::string m_UserName;
        std::string m_ExecutablePath;
        std::string m_StartupPath;
        std::string m_ProfilePath;
        std::string m_DesktopPath;
        std::string m_DocumentsPath;
        std::string m_DownloadsPath;
        std::string m_TemporaryPath;
    };

    extern UninitializedObject<WindowsEnvironmentStatics> GEnvironmentStatics;
}
