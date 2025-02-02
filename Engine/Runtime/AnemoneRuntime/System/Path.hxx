#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#include <string>
#include <string_view>

namespace Anemone::System
{
    class RUNTIME_API Path
    {
    public:
        static void AddDirectorySeparator(std::string& path);
        static void RemoveDirectorySeparator(std::string& path);

        static void NormalizeDirectorySeparators(std::string& path);
        static void Normalize(std::string& path);

        static void Push(std::string& path, std::string_view part);
        static bool Pop(std::string& path);
        static bool Pop(std::string& path, std::string& tail);

        static void SetExtension(std::string& path, std::string_view extension);
        static void SetFilename(std::string& path, std::string_view filename);

        static std::string_view GetExtension(std::string_view path);
        static std::string_view GetFilename(std::string_view path);

        // GetBaseName?
        static std::string_view GetFilenameWithoutExtension(std::string_view path);
    };
}
