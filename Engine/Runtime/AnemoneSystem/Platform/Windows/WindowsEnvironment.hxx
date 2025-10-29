#pragma once
#include "AnemoneSystem/Environment.hxx"

namespace Anemone
{
    struct WindowsEnvironment
    {
        static void Initialize();
        static void Finalize();
        static WindowsEnvironment& Get();
        
        DateTime startupTime{};
        Uuid systemId{};
        DeviceType deviceType{};
        DeviceProperties deviceProperties{};

        MemoryProperties memoryProperties{};

        std::string systemVersion{};
        std::string systemName{};
        std::string deviceId{};
        std::string deviceName{};
        std::string deviceModel{};
        std::string deviceManufacturer{};
        std::string deviceVersion{};
        std::string computerName{};
        std::string userName{};
        std::string executablePath{};
        std::string startupPath{};
        std::string profilePath{};
        std::string desktopPath{};
        std::string documentsPath{};
        std::string downloadsPath{};
        std::string temporaryPath{};
    };
}
