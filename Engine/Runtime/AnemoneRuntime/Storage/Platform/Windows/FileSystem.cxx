#include "AnemoneRuntime/Storage/Platform/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Storage/Platform/Windows/FileHandle.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/Debug.hxx"

namespace Anemone::Internal
{
    UninitializedObject<WindowsFileSystem> GPlatformFileSystem{};

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
            destination.Accessed = Interop::Windows::ToDateTime(source.ftLastAccessTime);
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

        [[maybe_unused]] bool InternalEnumerateDirectory(
            std::wstring& root,
            WIN32_FIND_DATAW& entry,
            FunctionRef<void(std::wstring& root, WIN32_FIND_DATAW& entry)> callback)
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

            return false;
        }

        bool InternalDeleteDirectoryRecursive(const std::wstring& root)
        {
            // Create search path pattern.
            std::wstring search = root;
            Interop::Windows::PathAddDirectorySeparator(search);
            search.append(L"*.*");

            WIN32_FIND_DATAW wfd;
            Interop::Windows::SafeFindFileHandle hDirectory{FindFirstFileW(search.c_str(), &wfd)};

            if (hDirectory)
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
                                return false;
                            }
                        }
                        else
                        {
                            // Recursively delete the directory.
                            if (not InternalDeleteDirectoryRecursive(file))
                            {
                                return false;
                            }
                        }
                    }
                    else
                    {
                        // Clear attributes.
                        if (not SetFileAttributesW(file.c_str(), 0))
                        {
                            return false;
                        }

                        // Delete the file.
                        if (not DeleteFileW(file.c_str()))
                        {
                            return false;
                        }
                    }
                } while (FindNextFileW(hDirectory.Get(), &wfd));

                // Remove the directory.
                if (not RemoveDirectoryW(root.c_str()))
                {
                    DWORD const dwError = GetLastError();

                    return dwError == ERROR_FILE_NOT_FOUND;
                }

                return true;
            }

            return false;
        }

        bool InternalDirectoryCreateRecursively(const std::wstring& path)
        {
            DWORD const dwAttributes = GetFileAttributesW(path.c_str());

            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
                // Path doesn't exist. Try to create parent recursively.
                std::wstring parent = path;
                Interop::Windows::PathPopFragment(parent);

                if (not InternalDirectoryCreateRecursively(parent))
                {
                    // Failed to create parent directory. Propagate error.
                    return false;
                }

                // Try to create directory.
                if (CreateDirectoryW(path.c_str(), nullptr))
                {
                    return true;
                }

                DWORD const dwError = GetLastError();

                return dwError == ERROR_ALREADY_EXISTS;
            }
            else
            {
                return (dwAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0;
            }
        }

    }

    WindowsFileSystem::WindowsFileSystem()
    {
        this->_generator.Randomize(42);
    }

    bool WindowsFileSystem::CreatePipe(std::unique_ptr<FileHandle>& reader, std::unique_ptr<FileHandle>& writer)
    {
        HANDLE hRead = nullptr;
        HANDLE hWrite = nullptr;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        if (::CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            reader = std::make_unique<WindowsFileHandle>(Interop::Windows::SafeFileHandle{hRead});
            writer = std::make_unique<WindowsFileHandle>(Interop::Windows::SafeFileHandle{hWrite});
            return true;
        }

        AE_VERIFY_WIN32(GetLastError());
        return false;
    }

    auto WindowsFileSystem::CreateFileReader(std::string_view path) -> std::unique_ptr<FileHandle>
    {
        constexpr DWORD dwCreationDisposition = OPEN_EXISTING;
        constexpr DWORD dwAccess = GENERIC_READ;
        constexpr DWORD dwShare = 0;
        constexpr DWORD dwFlags = 0;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE handle = INVALID_HANDLE_VALUE;

        if (GetACP() == CP_UTF8)
        {
            Interop::Windows::FilePathA upath{path};
            handle = CreateFileA(upath.c_str(), dwAccess, dwShare, &sa, dwCreationDisposition, dwFlags, nullptr);
        }
        else
        {
            Interop::Windows::FilePathW wpath{};
            HRESULT hr = Interop::Windows::WidenString(wpath, path);

            if (SUCCEEDED(hr))
            {
                CREATEFILE2_EXTENDED_PARAMETERS cf{
                    .dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
                    .dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
                    .dwFileFlags = dwFlags,
                    .dwSecurityQosFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                    .lpSecurityAttributes = &sa,
                    .hTemplateFile = nullptr,
                };

                handle = CreateFile2(
                    wpath.data(),
                    dwAccess,
                    dwShare,
                    dwCreationDisposition,
                    &cf);
            }
            else
            {
                AE_VERIFY_HRESULT(hr);
                return nullptr;
            }
        }

        if (handle != INVALID_HANDLE_VALUE)
        {
            return std::make_unique<WindowsFileHandle>(Interop::Windows::SafeFileHandle{handle});
        }

        AE_VERIFY_WIN32(GetLastError());
        return nullptr;
    }

    auto WindowsFileSystem::CreateFileWriter(std::string_view path) -> std::unique_ptr<FileHandle>
    {
        constexpr DWORD dwCreationDisposition = CREATE_ALWAYS;
        constexpr DWORD dwAccess = GENERIC_READ | GENERIC_WRITE;
        constexpr DWORD dwShare = 0;
        constexpr DWORD dwFlags = 0;

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE handle;

        if (GetACP() == CP_UTF8)
        {
            Interop::Windows::FilePathA upath{path};
            handle = CreateFileA(upath.c_str(), dwAccess, dwShare, &sa, dwCreationDisposition, dwFlags, nullptr);
        }
        else
        {
            Interop::Windows::FilePathW wpath{};
            HRESULT hr = Interop::Windows::WidenString(wpath, path);

            if (SUCCEEDED(hr))
            {
                CREATEFILE2_EXTENDED_PARAMETERS cf{
                    .dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
                    .dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
                    .dwFileFlags = dwFlags,
                    .dwSecurityQosFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                    .lpSecurityAttributes = &sa,
                    .hTemplateFile = nullptr,
                };

                handle = CreateFile2(
                    wpath.data(),
                    dwAccess,
                    dwShare,
                    dwCreationDisposition,
                    &cf);
            }
            else
            {
                AE_VERIFY_HRESULT(hr);
                return nullptr;
            }
        }

        if (handle != INVALID_HANDLE_VALUE)
        {
            return std::make_unique<WindowsFileHandle>(Interop::Windows::SafeFileHandle{handle});
        }

        AE_VERIFY_WIN32(GetLastError());
        return nullptr;
    }

    bool WindowsFileSystem::FileExists(std::string_view path)
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (HRESULT hr = Interop::Windows::WidenString(wsPath, path); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }


        //
        // Check if the file exists.
        //

        DWORD const dwAttributes = GetFileAttributesW(wsPath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            // Do not accept directories here.
            return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
        }

        AE_VERIFY_WIN32(GetLastError());
        return false;
    }

    bool WindowsFileSystem::FileDelete(std::string_view path)
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (HRESULT hr = Interop::Windows::WidenString(wsPath, path); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }

        //
        // Delete the file.
        //

        if (DeleteFileW(wsPath.c_str()))
        {
            // File was deleted successfully.
            return true;
        }

        DWORD const dwError = GetLastError();
        if (dwError == ERROR_FILE_NOT_FOUND)
        {
            // File does not exist. Therefore, it's deleted.
            return true;
        }

        AE_VERIFY_WIN32(dwError);
        return false;
    }

    bool WindowsFileSystem::FileCopy(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve)
    { //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsSource;

        if (HRESULT hr = Interop::Windows::WidenString(wsSource, source); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }

        Interop::Windows::FilePathW wsDestination;

        if (HRESULT hr = Interop::Windows::WidenString(wsDestination, destination); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }


        //
        // Check if the destination file already exists.
        //

        bool exists;

        if (DWORD const dwAttributes = GetFileAttributesW(wsDestination.c_str());
            dwAttributes == INVALID_FILE_ATTRIBUTES)
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_FILE_NOT_FOUND) or (dwError == ERROR_PATH_NOT_FOUND))
            {
                exists = false;
            }
            else
            {
                AE_VERIFY_WIN32(dwError);
                return false;
            }
        }
        else
        {
            exists = true;
        }


        //
        // Validate options.
        //

        bool failIfExists;

        if (exists)
        {
            switch (nameCollisionResolve)
            {
            default:
            case NameCollisionResolve::Fail:
                AE_TRACE(Error, "File copy failed: file already exists: {}", destination);
                return false;

            case NameCollisionResolve::Overwrite:
                failIfExists = false;
                break;

            case NameCollisionResolve::GenerateUnique:
                // TODO: Generate unique name. Maybe try to append system time?
                // failIfExists = false;
                AE_TRACE(Error, "File copy failed: generating unique name is not implemented: {}", destination);
                return false;
            }
        }
        else
        {
            failIfExists = true;
        }


        //
        // Prepare copying.
        //

        COPYFILE2_EXTENDED_PARAMETERS params{
            .dwSize = sizeof(params),
            .dwCopyFlags = COPY_FILE_NO_BUFFERING,
        };

        if (failIfExists)
        {
            params.dwCopyFlags |= COPY_FILE_FAIL_IF_EXISTS;
        }

        if (FAILED(CopyFile2(wsSource.c_str(), wsDestination.c_str(), &params)))
        {
            return false;
        }

        return true;
    }

    bool WindowsFileSystem::FileMove(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve)
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsSource;

        if (HRESULT hr = Interop::Windows::WidenString(wsSource, source); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }

        Interop::Windows::FilePathW wsDestination;

        if (HRESULT hr = Interop::Windows::WidenString(wsDestination, destination); FAILED(hr))
        {
            AE_VERIFY_HRESULT(hr);
            return false;
        }


        //
        // Check if the destination file already exists.
        //

        bool exists;

        if (DWORD const dwAttributes = GetFileAttributesW(wsDestination.c_str());
            dwAttributes == INVALID_FILE_ATTRIBUTES)
        {
            DWORD const dwError = GetLastError();

            if ((dwError == ERROR_FILE_NOT_FOUND) or (dwError == ERROR_PATH_NOT_FOUND))
            {
                exists = false;
            }
            else
            {
                AE_VERIFY_WIN32(dwError);
                return false;
            }
        }
        else
        {
            exists = true;
        }


        //
        // Validate options.
        //

        DWORD dwFlags = 0;

        if (exists)
        {
            switch (nameCollisionResolve)
            {
            default:
            case NameCollisionResolve::Fail:
                AE_TRACE(Error, "File move failed: file already exists: {}", destination);
                return false;

            case NameCollisionResolve::Overwrite:
                dwFlags |= MOVEFILE_REPLACE_EXISTING;
                break;

            case NameCollisionResolve::GenerateUnique:
                // TODO: Generate unique name. Maybe try to append system time?
                // dwFlags |= MOVEFILE_REPLACE_EXISTING;
                AE_TRACE(Error, "Not implemented: generating unique name for file move: {}", destination);
                return false;
            }
        }


        //
        // Prepare move operation.
        //

        if (MoveFileExW(wsSource.c_str(), wsDestination.c_str(), dwFlags))
        {
            return true;
        }

        return false;
    }

    auto WindowsFileSystem::GetPathInfo(std::string_view path) -> std::optional<FileInfo>
    {
        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return std::nullopt;
        }

        WIN32_FILE_ATTRIBUTE_DATA wfad;

        if (not GetFileAttributesExW(wsPath.c_str(), GetFileExInfoStandard, &wfad))
        {
            return std::nullopt;
        }

        FileInfo result;

        FileInfoFromSystem(wfad, result);

        return result;
    }

    // auto FileSystem::GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, Status>
    //{
    //    BY_HANDLE_FILE_INFORMATION bhfi;
    //
    //    if (not GetFileInformationByHandle(handle.GetNativeHandle().Get(), &bhfi))
    //    {
    //        return std::unexpected(Status::InvalidHandle);
    //    }
    //
    //    FileInfo result;
    //
    //    Internal::FileInfoFromSystem(bhfi, result);
    //
    //    return result;
    //}


    bool WindowsFileSystem::DirectoryExists(std::string_view path)
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return false;
        }

        //
        // Check if the directory exists.
        //

        DWORD const dwAttributes = GetFileAttributesW(wsPath.c_str());

        if (dwAttributes != INVALID_FILE_ATTRIBUTES)
        {
            // Do not accept files here.
            return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        }

        return false;
    }

    bool WindowsFileSystem::DirectoryDelete(std::string_view path)
    {
        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        if (RemoveDirectoryW(wsPath.c_str()))
        {
            return true;
        }

        DWORD const dwError = GetLastError();

        return dwError == ERROR_FILE_NOT_FOUND;
    }

    bool WindowsFileSystem::DirectoryDeleteRecursive(std::string_view path)
    {
        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        return InternalDeleteDirectoryRecursive(wsPath);
    }

    bool WindowsFileSystem::DirectoryCreate(std::string_view path)
    {
        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        if (CreateDirectoryW(wsPath.c_str(), nullptr))
        {
            return true;
        }

        DWORD const dwError = GetLastError();

        // Directory already exists?
        return dwError == ERROR_ALREADY_EXISTS;
    }

    bool WindowsFileSystem::DirectoryCreateRecursive(std::string_view path)
    {
        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        return InternalDirectoryCreateRecursively(wsPath);
    }

    bool WindowsFileSystem::DirectoryEnumerate(std::string_view path, FileSystemVisitor& visitor)
    {
        std::wstring root{};
        if (FAILED(Interop::Windows::WidenString(root, path)))
        {
            return false;
        }

        Interop::Windows::PathAddDirectorySeparator(root);
        root.push_back(L'*');

        WIN32_FIND_DATAW wfd;

        Interop::Windows::SafeFindFileHandle hFind{FindFirstFileW(root.c_str(), &wfd)};

        if (hFind)
        {
            do
            {
                if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    if (wcscmp(wfd.cFileName, L".") == 0 or wcscmp(wfd.cFileName, L"..") == 0)
                    {
                        continue;
                    }
                }

                FileInfo info;
                FileInfoFromSystem(wfd, info);

                Interop::string_buffer<char, 128> sFileName{};
                Interop::Windows::NarrowString(sFileName, wfd.cFileName);

                visitor.Visit(path, sFileName.as_view(), info);
            } while (FindNextFileW(hFind.Get(), &wfd));

            return true;
        }

        return false;
    }

    bool WindowsFileSystem::DirectoryEnumerateRecursive(std::string_view path, FileSystemVisitor& visitor)
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
