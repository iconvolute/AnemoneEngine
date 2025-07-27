#pragma once
#include "AnemoneRuntime/Storage/FileHandle.hxx"
#include "AnemoneRuntime/Base/DateTime.hxx"

#include <vector>
#include <string>
#include <memory>
#include <cstddef>

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
        DateTime Modified;
        int64_t Size;
        FileType Type;
        bool ReadOnly; // TODO: Replace with FileAttributes?
    };

    class FileSystemVisitor
    {
    public:
        FileSystemVisitor() = default;
        FileSystemVisitor(FileSystemVisitor const&) = default;
        FileSystemVisitor(FileSystemVisitor&&) = default;
        FileSystemVisitor& operator=(FileSystemVisitor const&) = default;
        FileSystemVisitor& operator=(FileSystemVisitor&&) = default;
        virtual ~FileSystemVisitor() = default;

    public:
        virtual void Visit(
            std::string_view path,
            std::string_view name,
            FileInfo const& info) = 0;
    };

    enum class NameCollisionResolve
    {
        Overwrite,
        Fail,
    };

    class FileSystem
    {
    public:
        FileSystem() = default;

        FileSystem(FileSystem const&) = delete;

        FileSystem(FileSystem&&) = delete;

        FileSystem& operator=(FileSystem const&) = delete;

        FileSystem& operator=(FileSystem&&) = delete;

        virtual ~FileSystem() = default;

    public:
        // gets platform file system
        RUNTIME_API static FileSystem& GetPlatformFileSystem();

    public:
        virtual auto CreatePipe(
            std::unique_ptr<FileHandle>& outReader,
            std::unique_ptr<FileHandle>& outWriter)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto CreateFile(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOption> options)
            -> std::expected<std::unique_ptr<FileHandle>, ErrorCode> = 0;

        virtual auto CreateFileReader(
            std::string_view path)
            -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>;

        virtual auto CreateFileWriter(
            std::string_view path)
            -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>;

        virtual auto ReadTextFile(
            std::string_view path)
            -> std::expected<std::string, ErrorCode>;

        virtual auto ReadBinaryFile(
            std::string_view path)
            -> std::expected<std::vector<std::byte>, ErrorCode>;

        virtual auto WriteTextFile(
            std::string_view path,
            std::string_view content)
            -> std::expected<void, ErrorCode>;

        virtual auto WriteBinaryFile(
            std::string_view path,
            std::span<std::byte const> content)
            -> std::expected<void, ErrorCode>;

        virtual auto GetPathInfo(
            std::string_view path)
            -> std::expected<FileInfo, ErrorCode> = 0;

        virtual auto Exists(
            std::string_view path)
            -> std::expected<bool, ErrorCode> = 0;

        virtual auto FileExists(
            std::string_view path)
            -> std::expected<bool, ErrorCode> = 0;

        virtual auto FileDelete(
            std::string_view path)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto FileCopy(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, ErrorCode>;

        virtual auto FileMove(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryExists(
            std::string_view path)
            -> std::expected<bool, ErrorCode> = 0;

        virtual auto DirectoryDelete(
            std::string_view path)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryDeleteRecursive(
            std::string_view path)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryCreate(
            std::string_view path)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryCreateRecursive(
            std::string_view path)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryEnumerate(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, ErrorCode> = 0;

        virtual auto DirectoryEnumerateRecursive(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, ErrorCode> = 0;
    };
}
