#pragma once
#include "AnemoneRuntime.Storage/FileSystem.hxx"

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
        auto CreateFileReader(
            std::string_view path)
            -> std::expected<Reference<FileHandle>, Error> override;

        auto CreateFileWriter(
            std::string_view path)
            -> std::expected<Reference<FileHandle>, Error> override;

        auto GetPathInfo(
            std::string_view path)
            -> std::expected<FileInfo, Error> override;

        auto Exists(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto FileExists(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto FileDelete(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto FileCopy(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, Error> override;

        auto FileMove(
            std::string_view source,
            std::string_view destination,
            NameCollisionResolve nameCollisionResolve)
            -> std::expected<void, Error> override;

        auto DirectoryExists(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto DirectoryDelete(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto DirectoryDeleteRecursive(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto DirectoryCreate(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto DirectoryCreateRecursive(
            std::string_view path)
            -> std::expected<void, Error> override;

        auto DirectoryEnumerate(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, Error> override;

        auto DirectoryEnumerateRecursive(
            std::string_view path,
            FileSystemVisitor& visitor)
            -> std::expected<void, Error> override;
    };
}
