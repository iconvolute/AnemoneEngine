#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    enum class FileType
    {
        Unknown,
        Directory,
        File,
        SymbolicLink,
        BlockDevice,
        CharacterDevice,
        Socket,
        NamedPipe,
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

    struct DirectoryVisitor
    {
        DirectoryVisitor() = default;
        DirectoryVisitor(DirectoryVisitor const&) = default;
        DirectoryVisitor(DirectoryVisitor&&) = default;
        DirectoryVisitor& operator=(DirectoryVisitor const&) = default;
        DirectoryVisitor& operator=(DirectoryVisitor&&) = default;
        virtual ~DirectoryVisitor() = default;
        virtual void Visit(std::string_view path, FileInfo const& info) = 0;
    };

    using DirectoryEnumerateFn = FunctionRef<void(std::string_view path, FileInfo const& info)>;

    enum class NameCollisionResolve
    {
        Overwrite,
        Fail,
        GenerateUnique,
    };

    struct FileSystem
    {
        friend struct Runtime;
        
    private:
        static void Initialize();
        static void Finalize();

    public:
        FileSystem() = delete;

        RUNTIME_API static auto FileExists(std::string_view path) -> std::expected<bool, ErrorCode>;

        RUNTIME_API static auto FileDelete(std::string_view path) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto FileCreate(std::string_view path) -> std::expected<FileHandle, ErrorCode>;

        RUNTIME_API static auto FileCopy(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto FileMove(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto GetFileInfo(std::string_view path) -> std::expected<FileInfo, ErrorCode>;

        RUNTIME_API static auto GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, ErrorCode>;

        RUNTIME_API static auto DirectoryExists(std::string_view path) -> std::expected<bool, ErrorCode>;

        RUNTIME_API static auto DirectoryDelete(std::string_view path, bool recursive) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto DirectoryCreate(std::string_view path, bool recursive) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto DirectoryEnumerate(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto DirectoryEnumerateRecursive(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, ErrorCode>;

        RUNTIME_API static auto DirectoryEnumerate(std::string_view path, DirectoryEnumerateFn fn) -> std::expected<void, ErrorCode>;
    };

    //
    // File System design
    // ==================
    //
    // The file system
    //
    //
}
