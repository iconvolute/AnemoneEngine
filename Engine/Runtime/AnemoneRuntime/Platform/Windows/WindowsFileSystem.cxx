#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

namespace Anemone
{
    auto FileSystem::FileExists(std::string_view path) -> std::expected<bool, ErrorCode>
    {
        Interop::win32_FilePathW wsPath;

        if (not Interop::win32_WidenString(wsPath, path))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD const dwAttributes = GetFileAttributesW(wsPath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            // Do not accept directories here.
            return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
        }

        return std::unexpected(ErrorCode::InvalidPath);
    }

    auto FileSystem::FileDelete(std::string_view path) -> std::expected<void, ErrorCode>
    {
        Interop::win32_FilePathW wsPath;

        if (not Interop::win32_WidenString(wsPath, path))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (DeleteFileW(wsPath.c_str()))
        {
            // File was deleted successfully.
            return {};
        }

        DWORD const dwError = GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND)
        {
            // File does not exist.
            return {};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    auto FileSystem::FileCreate(std::string_view path) -> std::expected<FileHandle, ErrorCode>
    {
        return FileHandle::Create(path, FileMode::CreateAlways);
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        bool overwrite) -> std::expected<void, ErrorCode>
    {
        Interop::win32_FilePathW wsSource;

        if (not Interop::win32_WidenString(wsSource, source))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        Interop::win32_FilePathW wsDestination;

        if (not Interop::win32_WidenString(wsDestination, destination))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (CopyFileW(wsSource.c_str(), wsDestination.c_str(), not overwrite))
        {
            return {};
        }

        DWORD const dwError = GetLastError();

        if (dwError == ERROR_FILE_EXISTS)
        {
            return std::unexpected(ErrorCode::FileExists);
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    auto FileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        bool overwrite) -> std::expected<void, ErrorCode>
    {
        Interop::win32_FilePathW wsSource;

        if (not Interop::win32_WidenString(wsSource, source))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        Interop::win32_FilePathW wsDestination;

        if (not Interop::win32_WidenString(wsDestination, destination))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD dwFlags = MOVEFILE_COPY_ALLOWED;

        if (overwrite)
        {
            dwFlags |= MOVEFILE_REPLACE_EXISTING;
        }

        if (MoveFileExW(wsSource.c_str(), wsDestination.c_str(), dwFlags))
        {
            return {};
        }

        return std::unexpected(ErrorCode::InvalidOperation);
    }

    auto FileSystem::GetFileInfo(std::string_view path) -> std::expected<FileInfo, ErrorCode>
    {
        Interop::win32_FilePathW wsPath;

        if (not Interop::win32_WidenString(wsPath, path))
        {
            return {};
        }

        WIN32_FILE_ATTRIBUTE_DATA wfad;

        if (not GetFileAttributesExW(wsPath.c_str(), GetFileExInfoStandard, &wfad))
        {
            return std::unexpected(ErrorCode::InvalidPath);
        }

        FileInfo result;

        result.Created = Interop::win32_into_DateTime(wfad.ftCreationTime);
        result.Accessed = Interop::win32_into_DateTime(wfad.ftLastAccessTime);
        result.Modified = Interop::win32_into_DateTime(wfad.ftLastWriteTime);

        if (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.Type = FileType::Directory;
            result.Size = 0;
        }
        else
        {
            result.Type = FileType::File;
            result.Size = static_cast<int64_t>((static_cast<DWORD64>(wfad.nFileSizeHigh) << 32) | wfad.nFileSizeLow);
        }

        result.ReadOnly = (wfad.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;

        return result;
    }

    auto FileSystem::GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, ErrorCode>
    {
        BY_HANDLE_FILE_INFORMATION bhfi;

        if (not GetFileInformationByHandle(handle.GetNativeHandle().Get(), &bhfi))
        {
            return std::unexpected(ErrorCode::InvalidHandle);
        }

        FileInfo result;

        result.Created = Interop::win32_into_DateTime(bhfi.ftCreationTime);
        result.Accessed = Interop::win32_into_DateTime(bhfi.ftLastAccessTime);
        result.Modified = Interop::win32_into_DateTime(bhfi.ftLastWriteTime);

        if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.Type = FileType::Directory;
            result.Size = 0;
        }
        else
        {
            result.Type = FileType::File;
            result.Size = (static_cast<int64_t>(bhfi.nFileSizeHigh) << 32) | bhfi.nFileSizeLow;
        }

        result.ReadOnly = (bhfi.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;

        return result;
    }

}
