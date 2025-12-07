#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <span>
#include <string>
#include <string_view>

namespace Anemone
{
    struct FilePath final
    {
        ANEMONE_RUNTIME_BASE_API static void AddDirectorySeparator(std::string& path);

        ANEMONE_RUNTIME_BASE_API static void RemoveDirectorySeparator(std::string& path);

        ANEMONE_RUNTIME_BASE_API static void NormalizeDirectorySeparators(std::string& path);

        ANEMONE_RUNTIME_BASE_API static void Normalize(std::string& path);

        ANEMONE_RUNTIME_BASE_API static void PushFragment(std::string& path, std::string_view part);

        ANEMONE_RUNTIME_BASE_API static bool PopFragment(std::string& path);

        ANEMONE_RUNTIME_BASE_API static bool PopFragment(std::string& path, std::string& tail);

        ANEMONE_RUNTIME_BASE_API static std::string_view GetFileName(std::string_view path);

        ANEMONE_RUNTIME_BASE_API static void SetFileName(std::string& path, std::string_view filename);

        ANEMONE_RUNTIME_BASE_API static std::string_view GetBaseName(std::string_view path);

        ANEMONE_RUNTIME_BASE_API static void SetBaseName(std::string& path, std::string_view baseName);

        ANEMONE_RUNTIME_BASE_API static std::string_view GetExtension(std::string_view path);

        ANEMONE_RUNTIME_BASE_API static void SetExtension(std::string& path, std::string_view extension);

        ANEMONE_RUNTIME_BASE_API static void MakeFileNameSafe(std::string& path);

        ANEMONE_RUNTIME_BASE_API static bool Relative(std::string& outRelativePath, std::string_view fromPath, std::string_view toPath);
    };
}
