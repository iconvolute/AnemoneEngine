#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/DateTime.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Internal
{
    template <typename SourceT>
    static void FileInfoFromSystem(
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
}

namespace Anemone
{
    auto FileSystem::FileExists(std::string_view path) -> std::expected<bool, Status>
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return std::unexpected(Status::InvalidArgument);
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

        return std::unexpected(Status::InvalidPath);
    }

    auto FileSystem::FileDelete(std::string_view path) -> std::expected<void, Status>
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        //
        // Delete the file.
        //

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

        return std::unexpected(Status::InvalidOperation);
    }

    auto FileSystem::FileCreate(std::string_view path) -> std::expected<FileHandle, Status>
    {
        return FileHandle::Create(path, FileMode::CreateAlways);
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve) -> std::expected<void, Status>
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsSource;

        if (FAILED(Interop::Windows::WidenString(wsSource, source)))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        Interop::Windows::FilePathW wsDestination;

        if (FAILED(Interop::Windows::WidenString(wsDestination, destination)))
        {
            return std::unexpected(Status::InvalidArgument);
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
                return std::unexpected(Status::InvalidOperation);
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
            case NameCollisionResolve::Fail:
                return std::unexpected(Status::FileExists);

            case NameCollisionResolve::Overwrite:
                failIfExists = false;
                break;

            case NameCollisionResolve::GenerateUnique:
                // TODO: Generate unique name. Maybe try to append system time?
                // failIfExists = false;
                return std::unexpected(Status::NotImplemented);

            default:
                return std::unexpected(Status::InvalidArgument);
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
            return std::unexpected(Status::AlreadyExists);
        }

        return std::unexpected(Status::InvalidOperation);
    }

    auto FileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve) -> std::expected<void, Status>
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsSource;

        if (FAILED(Interop::Windows::WidenString(wsSource, source)))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        Interop::Windows::FilePathW wsDestination;

        if (FAILED(Interop::Windows::WidenString(wsDestination, destination)))
        {
            return std::unexpected(Status::InvalidArgument);
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
                return std::unexpected(Status::InvalidOperation);
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
            case NameCollisionResolve::Fail:
                return std::unexpected(Status::FileExists);

            case NameCollisionResolve::Overwrite:
                dwFlags |= MOVEFILE_REPLACE_EXISTING;
                break;

            case NameCollisionResolve::GenerateUnique:
                // TODO: Generate unique name. Maybe try to append system time?
                // dwFlags |= MOVEFILE_REPLACE_EXISTING;
                return std::unexpected(Status::NotImplemented);

            default:
                return std::unexpected(Status::InvalidArgument);
            }
        }


        //
        // Prepare move operation.
        //

        if (MoveFileExW(wsSource.c_str(), wsDestination.c_str(), dwFlags))
        {
            return {};
        }

        return std::unexpected(Status::InvalidOperation);
    }

    auto FileSystem::GetFileInfo(std::string_view path) -> std::expected<FileInfo, Status>
    {
        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return {};
        }

        WIN32_FILE_ATTRIBUTE_DATA wfad;

        if (not GetFileAttributesExW(wsPath.c_str(), GetFileExInfoStandard, &wfad))
        {
            return std::unexpected(Status::InvalidPath);
        }

        FileInfo result;

        Internal::FileInfoFromSystem(wfad, result);

        return result;
    }

    auto FileSystem::GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, Status>
    {
        BY_HANDLE_FILE_INFORMATION bhfi;

        if (not GetFileInformationByHandle(handle.GetNativeHandle().Get(), &bhfi))
        {
            return std::unexpected(Status::InvalidHandle);
        }

        FileInfo result;

        Internal::FileInfoFromSystem(bhfi, result);

        return result;
    }

    auto FileSystem::DirectoryExists(std::string_view path) -> std::expected<bool, Status>
    {
        //
        // Convert paths to wide strings.
        //

        Interop::Windows::FilePathW wsPath;

        if (FAILED(Interop::Windows::WidenString(wsPath, path)))
        {
            return std::unexpected(Status::InvalidArgument);
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

        return std::unexpected(Status::InvalidPath);
    }

    static auto InternalDeleteDirectoryRecursive(const std::wstring& root) -> Status
    {
        //
        // Create search path pattern.
        //

        std::wstring search = root;
        Interop::Windows::PathAddDirectorySeparator(search);
        search.append(L"*.*");

        WIN32_FIND_DATAW wfd;

        Interop::Windows::SafeFindFileHandle hDirectory{FindFirstFileW(search.c_str(), &wfd)};

        if (hDirectory)
        {
            do
            {
                bool const isDirectory = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

                if (isDirectory)
                {
                    //
                    // Skip "." and ".." directories.
                    //

                    if (wcscmp(wfd.cFileName, L".") == 0 or wcscmp(wfd.cFileName, L"..") == 0)
                    {
                        continue;
                    }
                }

                //
                // Create full path to the file.
                //

                std::wstring file = root;
                Interop::Windows::PathAddDirectorySeparator(file);
                file.append(wfd.cFileName);


                if (isDirectory)
                {
                    //
                    // Delete the directory recursively.
                    //

                    bool const isSymlink = (wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

                    if (isSymlink)
                    {
                        //
                        // Remove the symlink.
                        //

                        if (not RemoveDirectoryW(file.c_str()))
                        {
                            return Status::InvalidOperation;
                        }
                    }
                    else
                    {
                        //
                        // Recursively delete the directory.
                        //

                        Status const error = InternalDeleteDirectoryRecursive(file);

                        if (error != Status::Success)
                        {
                            return error;
                        }
                    }
                }
                else
                {
                    //
                    // Clear attributes.
                    //

                    if (not SetFileAttributesW(file.c_str(), 0))
                    {
                        return Status::InvalidOperation;
                    }


                    //
                    // Delete the file.
                    //

                    if (not DeleteFileW(file.c_str()))
                    {
                        AE_ASSERT(false, "Failed to delete file"); // Something went wrong.
                        // return Status::InvalidOperation;
                    }
                }
            } while (FindNextFileW(hDirectory.Get(), &wfd));

            //
            // Remove the directory.
            //

            if (not RemoveDirectoryW(root.c_str()))
            {
                return Status::InvalidOperation;
            }

            return Status::Success;
        }

        return Status::InvalidPath;
    }

    auto FileSystem::DirectoryDelete(std::string_view path, bool recursive) -> std::expected<void, Status>
    {
        //
        // Convert paths to wide strings.
        //

        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        if (recursive)
        {
            Status const error = InternalDeleteDirectoryRecursive(wsPath);

            if (error == Status::Success)
            {
                return {};
            }

            return std::unexpected(error);
        }
        else
        {
            if (RemoveDirectoryW(wsPath.c_str()))
            {
                return {};
            }

            DWORD const dwError = GetLastError();
            if (dwError == ERROR_FILE_NOT_FOUND)
            {
                return {};
            }
            return std::unexpected(Status::DirectoryNotEmpty);
        }
    }

    static Status InternalDirectoryCreateRecursively(const std::wstring& path)
    {
        DWORD const dwAttributes = GetFileAttributesW(path.c_str());

        if (dwAttributes == INVALID_FILE_ATTRIBUTES)
        {
            //
            // Path doesn't exist. Try to create parent recursively.
            //

            std::wstring parent = path;
            Interop::Windows::PathPopFragment(parent);

            if (Status const result = InternalDirectoryCreateRecursively(parent);
                result != Status::Success)
            {
                //
                // Failed to create parent directory. Propagate error.
                //

                return result;
            }


            //
            // Try to create directory.
            //

            if (CreateDirectoryW(path.c_str(), nullptr))
            {
                return Status::Success;
            }

            DWORD const dwError = GetLastError();

            if (dwError == ERROR_ALREADY_EXISTS)
            {
                // Directory already exists.
                return Status::Success;
            }

            //
            // Cannot create directory.
            //

            return Status::PathNotFound;
        }
        else
        {
            if ((dwAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0)
            {
                //
                // Directory exists.
                //
                return Status::Success;
            }

            return Status::FileExists;
        }
    }

    auto FileSystem::DirectoryCreate(std::string_view path, bool recursive) -> std::expected<void, Status>
    {
        //
        // Convert paths to wide strings.
        //

        std::wstring wsPath;
        Interop::Windows::WidenString(wsPath, path);

        if (recursive)
        {
            Status const error = InternalDirectoryCreateRecursively(wsPath);

            if (error == Status::Success)
            {
                return {};
            }

            return std::unexpected(error);
        }
        else
        {
            //
            // Try to create directory.
            //

            if (CreateDirectoryW(wsPath.c_str(), nullptr))
            {
                return {};
            }

            DWORD const dwError = GetLastError();

            if (dwError == ERROR_ALREADY_EXISTS)
            {
                // Directory already exists.
                return std::unexpected(Status::Success);
            }

            return std::unexpected(Status::PathNotFound);
        }
    }

    auto FileSystem::DirectoryEnumerate(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, Status>
    {
        std::wstring root;
        if (FAILED(Interop::Windows::WidenString(root, path)))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        Interop::Windows::PathAddDirectorySeparator(root);
        root.push_back(L'*');

        WIN32_FIND_DATAW wfd;

        Interop::Windows::SafeFindFileHandle hFind{FindFirstFileW(root.c_str(), &wfd)};

        if (hFind)
        {
            do
            {
                bool const isDirectory = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

                if (isDirectory)
                {
                    if (wcscmp(wfd.cFileName, L".") == 0 or wcscmp(wfd.cFileName, L"..") == 0)
                    {
                        continue;
                    }
                }

                FileInfo info;
                Internal::FileInfoFromSystem(wfd, info);

                std::string fullPath{path};

                Interop::string_buffer<char, 128> sFileName{};
                Interop::Windows::NarrowString(sFileName, wfd.cFileName);

                FilePath::PushFragment(fullPath, sFileName.as_view());
                FilePath::NormalizeDirectorySeparators(fullPath);

                visitor.Visit(fullPath, info);
            } while (FindNextFileW(hFind.Get(), &wfd));

            return {};
        }

        return std::unexpected(Status::PathNotFound);
    }

    auto FileSystem::DirectoryEnumerateRecursive(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, Status>
    {
        class RecursiveVisitor final : public DirectoryVisitor
        {
        private:
            DirectoryVisitor& m_Visitor;

        public:
            RecursiveVisitor(DirectoryVisitor& visitor)
                : m_Visitor{visitor}
            {
            }

            void Visit(std::string_view path, FileInfo const& info) override
            {
                m_Visitor.Visit(path, info);

                if (info.Type == FileType::Directory)
                {
                    (void)FileSystem::DirectoryEnumerateRecursive(path, m_Visitor);
                }
            }
        };

        RecursiveVisitor recursiveVisitor{visitor};
        return FileSystem::DirectoryEnumerate(path, recursiveVisitor);
    }

    auto FileSystem::DirectoryEnumerate(std::string_view path, DirectoryEnumerateFn fn) -> std::expected<void, Status>
    {
        std::wstring root;
        if (FAILED(Interop::Windows::WidenString(root, path)))
        {
            return std::unexpected(Status::InvalidArgument);
        }

        Interop::Windows::PathAddDirectorySeparator(root);
        root.push_back(L'*');

        WIN32_FIND_DATAW wfd;

        if (Interop::Windows::SafeFindFileHandle hFind{FindFirstFileW(root.c_str(), &wfd)})
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
                Internal::FileInfoFromSystem(wfd, info);

                std::string fullPath{path};

                Interop::string_buffer<char, 128> sFileName{};
                Interop::Windows::NarrowString(sFileName, wfd.cFileName);

                FilePath::PushFragment(fullPath, sFileName.as_view());
                FilePath::NormalizeDirectorySeparators(fullPath);

                fn(fullPath, info);
            } while (FindNextFileW(hFind.Get(), &wfd));

            return {};
        }

        return std::unexpected(Status::PathNotFound);
    }
}
