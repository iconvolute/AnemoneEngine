#include "AnemoneRuntime/Storage/Platform/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Storage/Platform/Windows/FileHandle.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/Debug.hxx"

#include <system_error>

namespace Anemone::Internal
{
    namespace
    {
        UninitializedObject<WindowsFileSystem> GPlatformFileSystem{};
    }

    extern void PlatformInitializeFileSystem()
    {
        GPlatformFileSystem.Create();
    }

    extern void PlatformFinalizeFileSystem()
    {
        GPlatformFileSystem.Destroy();
    }
}

namespace Anemone
{
    FileSystem& FileSystem::GetPlatformFileSystem()
    {
        return *Internal::GPlatformFileSystem;
    }
}

namespace Anemone
{
    namespace
    {
        template <typename SourceT>
        void FileInfoFromSystem(
            SourceT const& source,
            FileInfo& destination)
        {
            destination.Created = Interop::Windows::ToDateTime(source.ftCreationTime);
            destination.Modified = Interop::Windows::ToDateTime(source.ftLastWriteTime);

            if (source.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                destination.Type = FileType::Directory;
                destination.Size = 0;
            }
            else if (source.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            {
                destination.Type = FileType::SymbolicLink;
                destination.Size = 0;
            }
            else
            {
                destination.Type = FileType::File;
                destination.Size = static_cast<int64_t>((static_cast<DWORD64>(source.nFileSizeHigh) << 32) | source.nFileSizeLow);
            }

            destination.ReadOnly = (source.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        }

        constexpr auto GetFileTypeFromFileAttributes(
            DWORD dwFileAttributes)
            -> FileType
        {
            if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                return FileType::Directory;
            }

            if (dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            {
                return FileType::SymbolicLink;
            }

            return FileType::File;
        }

        [[maybe_unused]] auto InternalEnumerateDirectory(
            std::wstring& root,
            WIN32_FIND_DATAW& entry,
            FunctionRef<void(std::wstring& root, WIN32_FIND_DATAW& entry)> callback)
            -> std::expected<bool, ErrorCode>
        {
            // Add search pattern to the root directory.
            Interop::Windows::PathAddDirectorySeparator(root);
            root.push_back(L'*');

            // Create a search handle for the directory.
            Interop::Windows::SafeFileHandle hFind{FindFirstFileW(root.c_str(), &entry)};

            // Remove the search pattern from the root directory. We need to pass the original root to the callback.
            Interop::Windows::PathPopFragment(root);

            if (hFind)
            {
                do
                {
                    if ((entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                    {
                        if ((wcscmp(entry.cFileName, L".") == 0) or (wcscmp(entry.cFileName, L"..") == 0))
                        {
                            // Skip "." and ".." directories.
                            continue;
                        }
                    }

                    callback(root, entry);
                } while (FindNextFileW(hFind.Get(), &entry));

                return true;
            }

            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_FILE_NOT_FOUND) or (dwError == ERROR_NO_MORE_FILES))
            {
                return false;
            }

            return std::unexpected(ErrorCode::Failure);
        }

        auto InternalDeleteDirectoryRecursive(
            const std::wstring& root)
            -> std::expected<void, ErrorCode>
        {
            // Create search path pattern.
            std::wstring search = root;
            Interop::Windows::PathAddDirectorySeparator(search);
            search.append(L"*.*");

            WIN32_FIND_DATAW wfd;
            Interop::Windows::SafeFindFileHandle enumerator{FindFirstFileW(search.c_str(), &wfd)};

            if (enumerator)
            {
                do
                {
                    bool const directory = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

                    if (directory)
                    {
                        // Skip "." and ".." directories.
                        if (wcscmp(wfd.cFileName, L".") == 0 or wcscmp(wfd.cFileName, L"..") == 0)
                        {
                            continue;
                        }
                    }

                    // Create full path to the file.
                    std::wstring file = root;
                    Interop::Windows::PathAddDirectorySeparator(file);
                    file.append(wfd.cFileName);


                    if (directory)
                    {
                        // Delete the directory recursively.
                        if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                        {
                            // Remove the symlink.
                            if (not RemoveDirectoryW(file.c_str()))
                            {
                                return std::unexpected(ErrorCode::Failure);
                            }
                        }
                        else
                        {
                            // Recursively delete the directory.
                            if (auto result = InternalDeleteDirectoryRecursive(file); not result)
                            {
                                return std::unexpected(result.error());
                            }
                        }
                    }
                    else
                    {
                        // Clear attributes.
                        if (not SetFileAttributesW(file.c_str(), 0))
                        {
                            return std::unexpected(ErrorCode::Failure);
                        }

                        // Delete the file.
                        if (not DeleteFileW(file.c_str()))
                        {
                            return std::unexpected(ErrorCode::Failure);
                        }
                    }
                } while (FindNextFileW(enumerator.Get(), &wfd));

                // Remove the directory.
                if (not RemoveDirectoryW(root.c_str()))
                {
                    return std::unexpected(ErrorCode::Failure);
                }

                return {};
            }

            return std::unexpected(ErrorCode::Failure);
        }

        auto InternalDirectoryCreateRecursively(
            const std::wstring& path)
            -> std::expected<void, ErrorCode>
        {
            if (path.empty())
            {
                return {};
            }

            DWORD const dwAttributes = GetFileAttributesW(path.c_str());

            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
                // Path doesn't exist. Try to create parent recursively.
                std::wstring parent = path;
                Interop::Windows::PathPopFragment(parent);

                if (not InternalDirectoryCreateRecursively(parent))
                {
                    // Failed to create parent directory. Propagate error.
                    return std::unexpected(ErrorCode::Failure);
                }

                // Try to create directory.
                if (not CreateDirectoryW(path.c_str(), nullptr))
                {
                    DWORD const dwError = GetLastError();

                    if (dwError == ERROR_ALREADY_EXISTS)
                    {
                        return {};
                    }

                    return std::unexpected(ErrorCode::Failure);
                }

                return {};
            }

            if ((dwAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0)
            {
                // Directory exists.
                return {};
            }

            return std::unexpected(ErrorCode::Failure);
        }

        constexpr auto TranslateCreationDisposition(
            FileMode mode)
            -> DWORD
        {
            switch (mode)
            {
            case FileMode::CreateAlways:
                return CREATE_ALWAYS;

            case FileMode::CreateNew:
                return CREATE_NEW;

            case FileMode::OpenAlways:
                return OPEN_ALWAYS;

            case FileMode::OpenExisting:
                return OPEN_EXISTING;

            case FileMode::TruncateExisting:
                return TRUNCATE_EXISTING;
            }

            return 0;
        }

        constexpr auto TranslateFileAccess(
            FileAccess access)
            -> DWORD
        {
            switch (access)
            {
            case FileAccess::Read:
                return GENERIC_READ;

            case FileAccess::Write:
                return GENERIC_WRITE;

            case FileAccess::ReadWrite:
                return GENERIC_READ | GENERIC_WRITE;
            }

            return 0;
        }

        constexpr auto TranslateFileShare(
            Flags<FileOption> options)
            -> DWORD
        {
            DWORD result = 0;

            if (options.Has(FileOption::ShareRead))
            {
                result |= FILE_SHARE_READ;
            }

            if (options.Has(FileOption::ShareWrite))
            {
                result |= FILE_SHARE_WRITE;
            }

            if (options.Has(FileOption::ShareDelete))
            {
                result |= FILE_SHARE_DELETE;
            }

            return result;
        }

        constexpr auto TranslateFileFlags(
            Flags<FileOption> options)
            -> DWORD
        {
            DWORD result = FILE_ATTRIBUTE_NORMAL;

            if (options.Has(FileOption::DeleteOnClose))
            {
                result |= FILE_FLAG_DELETE_ON_CLOSE;
            }

            if (options.Has(FileOption::RandomAccess))
            {
                result |= FILE_FLAG_RANDOM_ACCESS;
            }

            if (options.Has(FileOption::SequentialScan))
            {
                result |= FILE_FLAG_SEQUENTIAL_SCAN;
            }

            if (options.Has(FileOption::WriteThrough))
            {
                result |= FILE_FLAG_WRITE_THROUGH;
            }

            if (options.Has(FileOption::NoBuffering))
            {
                result |= FILE_FLAG_NO_BUFFERING;
            }

            if (options.Has(FileOption::Temporary))
            {
                result |= FILE_ATTRIBUTE_TEMPORARY;
            }

            return result;
        }

        [[maybe_unused]]
        constexpr auto TranslateWin32ErrorCode(
            DWORD error)
            -> ErrorCode
        {
            switch (error)
            {
            case ERROR_FILE_NOT_FOUND:
                return ErrorCode::FileNotFound;

            case ERROR_PATH_NOT_FOUND:
            case ERROR_BAD_NETPATH:
            case ERROR_CANT_RESOLVE_FILENAME:
            case ERROR_INVALID_DRIVE:
                return ErrorCode::PathNotFound;

            case ERROR_ACCESS_DENIED:
                return ErrorCode::AccessDenied;

            case ERROR_ALREADY_EXISTS:
            case ERROR_FILE_EXISTS:
                return ErrorCode::AlreadyExists;

            case ERROR_INVALID_NAME:
            case ERROR_DIRECTORY:
            case ERROR_FILENAME_EXCED_RANGE:
            case ERROR_BAD_PATHNAME:
                return ErrorCode::InvalidPath;

            case ERROR_FILE_READ_ONLY:
                return ErrorCode::AccessDenied;

            case ERROR_CANNOT_MAKE:
                return ErrorCode::InvalidOperation;

            case ERROR_DIR_NOT_EMPTY:
                return ErrorCode::DirectoryNotEmpty;

            case ERROR_WRITE_FAULT:
            case ERROR_READ_FAULT:
                return ErrorCode::IoError;

            case ERROR_LOCK_VIOLATION:
            case ERROR_SHARING_VIOLATION:
                return ErrorCode::InvalidOperation;

            case ERROR_HANDLE_EOF:
            case ERROR_HANDLE_DISK_FULL:
            case ERROR_DISK_FULL:
                return ErrorCode::Failure;

            default:
                return ErrorCode::Failure;
            }
        }
    }

    WindowsFileSystem::WindowsFileSystem()
    {
        this->_generator.Randomize(42);
    }

    auto WindowsFileSystem::CreatePipe(
        std::unique_ptr<FileHandle>& outReader,
        std::unique_ptr<FileHandle>& outWriter)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        SafeFileHandle nativeReader{};
        SafeFileHandle nativeWriter{};

        SECURITY_ATTRIBUTES securityAttributes{
            .nLength = sizeof(securityAttributes),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        if (::CreatePipe(nativeReader.GetAddressOf(), nativeWriter.GetAddressOf(), &securityAttributes, 0))
        {
            outReader = std::make_unique<WindowsFileHandle>(std::move(nativeReader));
            outWriter = std::make_unique<WindowsFileHandle>(std::move(nativeWriter));
            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::CreateFile(
        std::string_view path,
        FileMode mode,
        Flags<FileAccess> access,
        Flags<FileOption> options)
        -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>
    {
        DWORD const dwCreationDisposition = TranslateCreationDisposition(mode);
        DWORD const dwAccess = TranslateFileAccess(access);
        DWORD const dwShare = TranslateFileShare(options);
        DWORD const dwFlags = TranslateFileFlags(options);

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        using namespace Interop::Windows;

        FilePathW nativePath{};
        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        CREATEFILE2_EXTENDED_PARAMETERS cf{
            .dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
            .dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
            .dwFileFlags = dwFlags,
            .dwSecurityQosFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
            .lpSecurityAttributes = &sa,
            .hTemplateFile = nullptr,
        };

        HANDLE const handle = CreateFile2(
            nativePath.c_str(),
            dwAccess,
            dwShare,
            dwCreationDisposition,
            &cf);

        if (handle != INVALID_HANDLE_VALUE)
        {
            return std::make_unique<WindowsFileHandle>(SafeFileHandle{handle});
        }

        switch (GetLastError())
        {
        case ERROR_FILE_EXISTS:
            return std::unexpected(ErrorCode::AlreadyExists);
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::Exists(
        std::string_view path)
        -> std::expected<bool, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD const dwAttributes = GetFileAttributesW(nativePath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            return true;
        }

        DWORD const dwError = GetLastError();

        switch (dwError)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_NOT_READY:
        case ERROR_INVALID_DRIVE:
            return false;

        default:
            break;
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::FileExists(
        std::string_view path)
        -> std::expected<bool, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD const dwAttributes = GetFileAttributesW(nativePath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            if (GetFileTypeFromFileAttributes(dwAttributes) == FileType::File)
            {
                return true;
            }

            return std::unexpected(ErrorCode::InvalidFile);
        }

        DWORD const dwError = GetLastError();

        switch (dwError)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_NOT_READY:
        case ERROR_INVALID_DRIVE:
            return false;

        default:
            break;
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::FileDelete(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (DeleteFileW(nativePath.c_str()))
        {
            return {};
        }

        DWORD const dwError = GetLastError();

        if (dwError == ERROR_FILE_NOT_FOUND)
        {
            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativeSource{};
        FilePathW nativeDestination{};

        if (FAILED(WidenString(nativeSource, source)) or
            FAILED(WidenString(nativeDestination, destination)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        bool destinationExists = true;

        if (DWORD const dwAttributes = GetFileAttributesW(nativeDestination.c_str()); dwAttributes == INVALID_FILE_ATTRIBUTES)
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_FILE_NOT_FOUND) or (dwError == ERROR_PATH_NOT_FOUND))
            {
                destinationExists = false;
            }
            else
            {
                return std::unexpected(ErrorCode::InvalidPath);
            }
        }

        bool overwrite = false;

        if (destinationExists)
        {
            switch (nameCollisionResolve)
            {
            case NameCollisionResolve::Fail:
                return std::unexpected(ErrorCode::AlreadyExists);

            case NameCollisionResolve::Overwrite:
                overwrite = true;
                break;
            }
        }

        COPYFILE2_EXTENDED_PARAMETERS copyFileParameters{
            .dwSize = sizeof(copyFileParameters),
            .dwCopyFlags = COPY_FILE_NO_BUFFERING,
        };

        if (not overwrite)
        {
            copyFileParameters.dwCopyFlags |= COPY_FILE_FAIL_IF_EXISTS;
        }

        switch (CopyFile2(nativeSource.c_str(), nativeDestination.c_str(), &copyFileParameters))
        {
        case HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS):
        case HRESULT_FROM_WIN32(ERROR_FILE_EXISTS):
            return std::unexpected(ErrorCode::AlreadyExists);

        case S_OK:
            return {};

        default:
            break;
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativeSource{};
        FilePathW nativeDestination{};

        if (FAILED(WidenString(nativeSource, source)) or
            FAILED(WidenString(nativeDestination, destination)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        bool destinationExists = true;

        if (DWORD const dwAttributes = GetFileAttributesW(nativeDestination.c_str()); dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_FILE_NOT_FOUND) or (dwError == ERROR_PATH_NOT_FOUND))
            {
                destinationExists = false;
            }
            else
            {
                return std::unexpected(ErrorCode::InvalidPath);
            }
        }

        bool overwrite = false;

        if (destinationExists)
        {
            switch (nameCollisionResolve)
            {
            case NameCollisionResolve::Fail:
                return std::unexpected(ErrorCode::AlreadyExists);

            case NameCollisionResolve::Overwrite:
                overwrite = true;
                break;
            }
        }

        DWORD dwFlags = 0;

        if (destinationExists)
        {
            switch (nameCollisionResolve)
            {
            case NameCollisionResolve::Fail:
                return std::unexpected(ErrorCode::AlreadyExists);

            case NameCollisionResolve::Overwrite:
                overwrite = true;
                break;
            }
        }

        if (overwrite)
        {
            dwFlags |= MOVEFILE_REPLACE_EXISTING;
        }

        if (MoveFileExW(nativeSource.c_str(), nativeDestination.c_str(), dwFlags))
        {
            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::DirectoryExists(
        std::string_view path)
        -> std::expected<bool, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        DWORD const dwAttributes = GetFileAttributesW(nativePath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            if (GetFileTypeFromFileAttributes(dwAttributes) == FileType::Directory)
            {
                return true;
            }

            return std::unexpected(ErrorCode::InvalidDirectory);
        }

        DWORD const dwError = GetLastError();

        switch (dwError)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_NOT_READY:
        case ERROR_INVALID_DRIVE:
            return false;

        default:
            break;
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::GetPathInfo(
        std::string_view path)
        -> std::expected<FileInfo, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        WIN32_FILE_ATTRIBUTE_DATA nativeResult;

        if (GetFileAttributesExW(nativePath.c_str(), GetFileExInfoStandard, &nativeResult))
        {
            FileInfo result;
            FileInfoFromSystem(nativeResult, result);
            return result;
        }

        DWORD const dwError = GetLastError();

        if (dwError == ERROR_FILE_NOT_FOUND)
        {
            return std::unexpected(ErrorCode::NotFound);
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::DirectoryDelete(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};
        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (RemoveDirectoryW(nativePath.c_str()))
        {
            return {};
        }

        DWORD const dwError = GetLastError();

        if (dwError == ERROR_FILE_NOT_FOUND)
        {
            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::DirectoryDeleteRecursive(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        std::wstring nativePath;

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        return InternalDeleteDirectoryRecursive(nativePath);
    }

    auto WindowsFileSystem::DirectoryCreate(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        FilePathW nativePath{};

        if (FAILED(WidenString(nativePath, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (CreateDirectoryW(nativePath.c_str(), nullptr))
        {
            return {};
        }

        DWORD const dwError = GetLastError();

        if (dwError == ERROR_ALREADY_EXISTS)
        {
            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::DirectoryCreateRecursive(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        return InternalDirectoryCreateRecursively(wsPath);
    }

    auto WindowsFileSystem::DirectoryEnumerate(
        std::string_view path,
        FileSystemVisitor& visitor)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Windows;

        std::wstring root{};

        if (FAILED(WidenString(root, path)))
        {
            return std::unexpected(ErrorCode::InvalidArgument);
        }

        PathAddDirectorySeparator(root);
        root.push_back(L'*');

        WIN32_FIND_DATAW nativeFileInfo;

        SafeFindFileHandle enumerator{FindFirstFileW(root.c_str(), &nativeFileInfo)};

        if (enumerator)
        {
            do
            {
                if ((nativeFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    if (wcscmp(nativeFileInfo.cFileName, L".") == 0 or wcscmp(nativeFileInfo.cFileName, L"..") == 0)
                    {
                        continue;
                    }
                }

                FileInfo info;
                FileInfoFromSystem(nativeFileInfo, info);

                Interop::string_buffer<char, 128> sFileName{};
                NarrowString(sFileName, nativeFileInfo.cFileName);

                visitor.Visit(path, sFileName.as_view(), info);
            } while (FindNextFileW(enumerator.Get(), &nativeFileInfo));

            return {};
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto WindowsFileSystem::DirectoryEnumerateRecursive(
        std::string_view path,
        FileSystemVisitor& visitor)
        -> std::expected<void, ErrorCode>
    {
        class RecursiveVisitor final : public FileSystemVisitor
        {
        private:
            WindowsFileSystem* _owner{};
            FileSystemVisitor* _visitor{};

        public:
            RecursiveVisitor(
                WindowsFileSystem* owner,
                FileSystemVisitor& visitor)
                : _owner{owner}
                , _visitor{&visitor}
            {
            }

            void Visit(std::string_view path, std::string_view name, FileInfo const& info) override
            {
                this->_visitor->Visit(path, name, info);

                if (info.Type == FileType::Directory)
                {
                    (void)this->_owner->DirectoryEnumerate(path, *this->_visitor);
                }
            }
        };

        RecursiveVisitor recursiveVisitor{this, visitor};
        return this->DirectoryEnumerate(path, recursiveVisitor);
    }
}
