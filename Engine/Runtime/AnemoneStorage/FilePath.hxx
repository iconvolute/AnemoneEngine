#pragma once
#include "AnemoneInterop/Headers.hxx"

#include <span>
#include <string>
#include <string_view>

namespace Anemone
{
    struct FilePath final
    {
        RUNTIME_API static void AddDirectorySeparator(std::string& path);

        RUNTIME_API static void RemoveDirectorySeparator(std::string& path);

        RUNTIME_API static void NormalizeDirectorySeparators(std::string& path);

        RUNTIME_API static void Normalize(std::string& path);

        RUNTIME_API static void PushFragment(std::string& path, std::string_view part);

        RUNTIME_API static bool PopFragment(std::string& path);

        RUNTIME_API static bool PopFragment(std::string& path, std::string& tail);

        RUNTIME_API static std::string_view GetFileName(std::string_view path);

        RUNTIME_API static void SetFileName(std::string& path, std::string_view filename);

        RUNTIME_API static std::string_view GetBaseName(std::string_view path);

        RUNTIME_API static void SetBaseName(std::string& path, std::string_view baseName);

        RUNTIME_API static std::string_view GetExtension(std::string_view path);

        RUNTIME_API static void SetExtension(std::string& path, std::string_view extension);

        RUNTIME_API static void MakeFileNameSafe(std::string& path);
    };
}
