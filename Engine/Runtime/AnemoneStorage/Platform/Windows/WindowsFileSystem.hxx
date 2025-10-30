#pragma once
#include "AnemoneStorage/FileSystem.hxx"
#include "AnemoneRandom/Generator.hxx"

namespace Anemone
{
    class WindowsFileHandle;

    class WindowsFileSystem : public FileSystem
    {
    public:
        WindowsFileSystem();

        WindowsFileSystem(WindowsFileSystem const&) = delete;

        WindowsFileSystem(WindowsFileSystem&&) = delete;

        WindowsFileSystem& operator=(WindowsFileSystem const&) = delete;

        WindowsFileSystem& operator=(WindowsFileSystem&&) = delete;

        ~WindowsFileSystem() override = default;

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
