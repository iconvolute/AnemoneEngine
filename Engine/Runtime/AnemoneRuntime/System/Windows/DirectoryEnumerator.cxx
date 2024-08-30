#include "AnemoneRuntime/System/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/System/Path.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System::Private
{
    constexpr FileType ConvertToFileType(DWORD attributes)
    {
        if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
        {
            return FileType::SymbolicLink;
        }

        if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return FileType::Directory;
        }

        return FileType::File;
    }

    static DirectoryEntry FromNative(std::string_view root, WIN32_FIND_DATAW const& wfd)
    {
        Platform::win32_string_buffer<char, 512> narrow{};
        Platform::win32_NarrowString(narrow, wfd.cFileName);

        std::string path{root};
        Path::Push(path, narrow.as_view());

        return DirectoryEntry{
            .Path = std::move(path),
            .Type = ConvertToFileType(wfd.dwFileAttributes),
        };
    }
}

namespace Anemone::System
{
    DirectoryEnumerator::DirectoryEnumerator(std::string root)
        : _root{std::move(root)}
    {
        Platform::Create(this->_native, Platform::NativeDirectoryEnumerator{INVALID_HANDLE_VALUE});
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other) noexcept
        : _root{std::move(other._root)}
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), Platform::NativeDirectoryEnumerator{INVALID_HANDLE_VALUE}));
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);
            Platform::NativeDirectoryEnumerator& nativeOther = Platform::Get(other._native);

            if (nativeThis.Handle != INVALID_HANDLE_VALUE)
            {
                if (not FindClose(nativeThis.Handle))
                {
                    AE_BUGCHECK("Failed to close directory enumerator");
                }
            }

            this->_root = std::exchange(other._root, {});
            nativeThis = std::exchange(nativeOther, Platform::NativeDirectoryEnumerator{INVALID_HANDLE_VALUE});
        }

        return *this;
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle != INVALID_HANDLE_VALUE)
        {
            if (not FindClose(nativeThis.Handle))
            {
                AE_BUGCHECK("Failed to close directory enumerator");
            }
        }

        Platform::Destroy(this->_native);
    }

    std::optional<std::expected<DirectoryEntry, ErrorCode>> DirectoryEnumerator::Next()
    {
        Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);

        WIN32_FIND_DATAW wfd;

        if (nativeThis.Handle == INVALID_HANDLE_VALUE)
        {
            // TODO This should be possible to using a Path functions.
            std::wstring wpath{};

            if (Platform::win32_WidenString(wpath, this->_root))
            {
                if (not wpath.empty())
                {
                    Platform::win32_PathAddDirectorySeparator(wpath);

                    wpath += L'*';

                    if (HANDLE const handle = FindFirstFileW(wpath.c_str(), &wfd); handle != INVALID_HANDLE_VALUE)
                    {
                        nativeThis.Handle = handle;

                        return Private::FromNative(this->_root, wfd);
                    }

                    if (DWORD const dwError = GetLastError(); dwError == ERROR_FILE_NOT_FOUND)
                    {
                        return std::nullopt;
                    }
                    else
                    {
                        return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
                    }
                }
            }

            return std::unexpected(ErrorCode::InvalidArgument);
        }

        if (FindNextFileW(nativeThis.Handle, &wfd) != FALSE)
        {
            return Private::FromNative(this->_root, wfd);
        }

        if (DWORD const dwError = GetLastError(); dwError == ERROR_NO_MORE_FILES)
        {
            return std::nullopt;
        }
        else
        {
            return std::unexpected(Private::ErrorCodeFromWin32Error(dwError));
        }
    }

    std::string_view DirectoryEnumerator::Root() const
    {
        return this->_root;
    }
}
