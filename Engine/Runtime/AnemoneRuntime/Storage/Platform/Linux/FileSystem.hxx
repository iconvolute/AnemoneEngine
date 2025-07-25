#pragma once
#include "AnemoneRuntime/Storage/FileSystem.hxx"

namespace Anemone
{
    class LinuxFileHandle;

    class LinuxFileSystem : public FileSystem
    {
    public:
        LinuxFileSystem();
        
        LinuxFileSystem(LinuxFileSystem const&) = delete;
        
        LinuxFileSystem(LinuxFileSystem&&) = delete;
        
        LinuxFileSystem& operator=(LinuxFileSystem const&) = delete;
        
        LinuxFileSystem& operator=(LinuxFileSystem&&) = delete;
        
        ~LinuxFileSystem() override = default;

    public:
        auto CreatePipe(
            std::unique_ptr<FileHandle>& outReader,
            std::unique_ptr<FileHandle>& outWriter)
            -> std::expected<void, ErrorCode> override;

        auto CreateFile(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOption> options)
            -> std::expected<std::unique_ptr<FileHandle>, ErrorCode> override;

        auto GetPathInfo(
            std::string_view path)
            -> std::expected<FileInfo, ErrorCode> override;

        auto Exists(
            std::string_view path)
            -> std::expected<bool, ErrorCode> override;

        auto FileDelete(
            std::string_view path)
            -> std::expected<void, ErrorCode> override;

        auto FileCopy(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, ErrorCode> override;

        auto FileMove(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryDelete(
            std::string_view path)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryDeleteRecursive(
            std::string_view path)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryCreate(
            std::string_view path)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryCreateRecursive(
            std::string_view path)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryEnumerate(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, ErrorCode> override;

        auto DirectoryEnumerateRecursive(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, ErrorCode> override;

    };
}
