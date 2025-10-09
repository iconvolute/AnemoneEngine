#pragma once
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone
{
    struct LinuxEnvironment
    {
        static void Initialize(bool applicationType);
        static void Finalize();
        static LinuxEnvironment& Get();

        bool applicationType{};

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
