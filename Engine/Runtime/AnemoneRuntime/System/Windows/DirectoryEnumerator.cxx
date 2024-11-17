#include "AnemoneRuntime/System/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
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
        : m_root{std::move(root)}
        , m_native{INVALID_HANDLE_VALUE}
    {
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other) noexcept
        : m_root{std::exchange(other.m_root, {})}
        , m_native{std::exchange(other.m_native, {INVALID_HANDLE_VALUE})}
    {
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->m_native.Handle != INVALID_HANDLE_VALUE)
            {
                if (not FindClose(this->m_native.Handle))
                {
                    AE_PANIC("Failed to close directory enumerator");
                }
            }

            this->m_root = std::exchange(other.m_root, {});
            this->m_native = std::exchange(other.m_native, Platform::NativeDirectoryEnumerator{INVALID_HANDLE_VALUE});
        }

        return *this;
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        if (this->m_native.Handle != INVALID_HANDLE_VALUE)
        {
            if (not FindClose(this->m_native.Handle))
            {
                AE_PANIC("Failed to close directory enumerator");
            }
        }
    }

    std::optional<std::expected<DirectoryEntry, ErrorCode>> DirectoryEnumerator::Next()
    {
        WIN32_FIND_DATAW wfd;

        if (this->m_native.Handle == INVALID_HANDLE_VALUE)
        {
            // TODO This should be possible to using a Path functions.
            std::wstring wpath{};

            if (Platform::win32_WidenString(wpath, this->m_root))
            {
                if (not wpath.empty())
                {
                    Platform::win32_PathAddDirectorySeparator(wpath);

                    wpath += L'*';

                    if (HANDLE const handle = FindFirstFileW(wpath.c_str(), &wfd); handle != INVALID_HANDLE_VALUE)
                    {
                        this->m_native.Handle = handle;

                        return Private::FromNative(this->m_root, wfd);
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

        if (FindNextFileW(this->m_native.Handle, &wfd) != FALSE)
        {
            return Private::FromNative(this->m_root, wfd);
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
        return this->m_root;
    }
}
