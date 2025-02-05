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
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        bool overwrite) -> std::expected<void, ErrorCode>
    {
        (void)source;
        (void)destination;
        (void)overwrite;
        AE_PANIC("Not implemented");
        return std::unexpected(ErrorCode::NotImplemented);
    }

    auto FileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        bool overwrite) -> std::expected<void, ErrorCode>
    {
        (void)source;
        (void)destination;
        (void)overwrite;
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

}
