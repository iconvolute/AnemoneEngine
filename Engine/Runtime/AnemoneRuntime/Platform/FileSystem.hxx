#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/Platform/FileHandle.hxx"
#include "AnemoneRuntime/DateTime.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    enum class FileType
    {
        File,
        Directory,
    };

    struct FileInfo
    {
        DateTime Created;
        DateTime Accessed;
        DateTime Modified;
        int64_t Size;
        FileType Type;
        bool ReadOnly;
    };

    struct FileSystem
    {
        RUNTIME_API static auto FileExists(std::string_view path) -> std::expected<bool, ErrorCode>;

        RUNTIME_API static auto FileDelete(std::string_view path) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto FileCreate(std::string_view path) -> std::expected<FileHandle, ErrorCode>;

        RUNTIME_API static auto FileCopy(std::string_view source, std::string_view destination, bool overwrite) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto FileMove(std::string_view source, std::string_view destination, bool overwrite) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto GetFileInfo(std::string_view path) -> std::expected<FileInfo, ErrorCode>;

        RUNTIME_API static auto GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, ErrorCode>;
    };

    //
    // File System design
    // ==================
    //
    // The file system
    //
    //
}
