#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    auto FileSystem::FileExists(std::string_view path) -> std::expected<bool, ErrorCode>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::FileDelete(std::string_view path) -> std::expected<void, ErrorCode>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::FileCreate(std::string_view path) -> std::expected<FileHandle, ErrorCode>
    {
        return FileHandle::Create(path, FileMode::CreateAlways);
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve) -> std::expected<void, ErrorCode>
    {
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve) -> std::expected<void, ErrorCode>
    {
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::GetFileInfo(std::string_view path) -> std::expected<FileInfo, ErrorCode>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, ErrorCode>
    {
        (void)handle;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::DirectoryExists(std::string_view path) -> std::expected<bool, ErrorCode>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::DirectoryDelete(std::string_view path, bool recursive) -> std::expected<void, ErrorCode>
    {
        (void)path;
        (void)recursive;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::DirectoryCreate(std::string_view path, bool recursive) -> std::expected<void, ErrorCode>
    {
        (void)path;
        (void)recursive;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::DirectoryEnumerate(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, ErrorCode>
    {
        (void)path;
        (void)visitor;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::DirectoryEnumerateRecursive(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, ErrorCode>
    {
        (void)path;
        (void)visitor;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }
}
