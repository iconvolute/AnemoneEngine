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
        DateTime Accessed;
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
        virtual void Visit(std::string_view path, std::string_view name, FileInfo const& info) = 0;
    };

    enum class NameCollisionResolve
    {
        Overwrite,
        Fail,
        GenerateUnique,
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
        virtual bool CreatePipe(std::unique_ptr<FileHandle>& reader, std::unique_ptr<FileHandle>& writer) = 0;

        virtual std::unique_ptr<FileHandle> CreateFileReader(std::string_view path) = 0;

        virtual std::unique_ptr<FileHandle> CreateFileWriter(std::string_view path) = 0;

        // TODO: Consider returning some refcounted buffer for text and binary data. It might be helpful for custom package formats.
        virtual std::expected<std::string, Status> ReadTextFile(std::string_view path);

        virtual std::expected<std::vector<std::byte>, Status> ReadBinaryFile(std::string_view path);

        virtual std::expected<void, Status> WriteTextFile(std::string_view path, std::string_view content);

        virtual std::expected<void, Status> WriteBinaryFile(std::string_view path, std::span<std::byte const> content);

        virtual bool FileExists(std::string_view path) = 0;

        virtual bool FileDelete(std::string_view path) = 0;

        virtual bool FileCopy(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) = 0;

        virtual bool FileMove(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) = 0;

        virtual auto GetPathInfo(std::string_view path) -> std::optional<FileInfo> = 0;

        virtual bool DirectoryExists(std::string_view path) = 0;

        virtual bool DirectoryDelete(std::string_view path) = 0;

        virtual bool DirectoryDeleteRecursive(std::string_view path) = 0;

        virtual bool DirectoryCreate(std::string_view path) = 0;

        virtual bool DirectoryCreateRecursive(std::string_view path) = 0;

        virtual bool DirectoryEnumerate(std::string_view path, FileSystemVisitor& visitor) = 0;

        virtual bool DirectoryEnumerateRecursive(std::string_view path, FileSystemVisitor& visitor) = 0;
    };
}
