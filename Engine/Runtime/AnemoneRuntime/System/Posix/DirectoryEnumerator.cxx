#include "AnemoneRuntime/System/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"

#include <string>

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <dirent.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::System::Private
{
    constexpr FileType posix_into_FileType(dirent const& d)
    {
        switch (d.d_type)
        {
        default:
        case DT_UNKNOWN:
            return FileType::Unknown;

        case DT_FIFO:
            return FileType::NamedPipe;

        case DT_CHR:
            return FileType::CharacterDevice;

        case DT_DIR:
            return FileType::Directory;

        case DT_BLK:
            return FileType::BlockDevice;

        case DT_REG:
            return FileType::File;

        case DT_LNK:
            return FileType::SymbolicLink;

        case DT_SOCK:
            return FileType::Socket;
        }
    }

    constexpr FileType posix_into_FileType(struct stat64 const& v)
    {
        switch (v.st_mode & S_IFMT)
        {
        case S_IFSOCK:
            return FileType::Socket;

        case S_IFLNK:
            return FileType::SymbolicLink;

        case S_IFREG:
            return FileType::File;

        case S_IFBLK:
            return FileType::BlockDevice;

        case S_IFDIR:
            return FileType::Directory;

        case S_IFCHR:
            return FileType::CharacterDevice;

        case S_IFIFO:
            return FileType::NamedPipe;

        default:
            return FileType::Unknown;
        }
    }
}

namespace Anemone::System
{
    DirectoryEnumerator::DirectoryEnumerator(std::string root)
        : _root{std::move(root)}
    {
        Platform::Create(this->_native, Platform::NativeDirectoryEnumerator{nullptr});
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other) noexcept
        : _root{std::move(other._root)}
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), Platform::NativeDirectoryEnumerator{nullptr}));
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);
            Platform::NativeDirectoryEnumerator& nativeOther = Platform::Get(other._native);

            if (nativeThis.Handle != nullptr)
            {
                if (closedir(nativeThis.Handle) != 0)
                {
                    AE_BUGCHECK("Failed to close directory enumerator (errno = {})", errno);
                }
            }

            this->_root = std::exchange(other._root, {});
            nativeThis = std::exchange(nativeOther, Platform::NativeDirectoryEnumerator{nullptr});
        }

        return *this;
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle != nullptr)
        {
            if (closedir(nativeThis.Handle) != 0)
            {
                AE_BUGCHECK("Failed to close directory enumerator (errno = {})", errno);
            }
        }

        Platform::Destroy(this->_native);
    }

    std::optional<std::expected<DirectoryEntry, ErrorCode>> DirectoryEnumerator::Next()
    {
        Platform::NativeDirectoryEnumerator& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle == nullptr)
        {
            DIR* handle = opendir(this->_root.c_str());

            if (handle != nullptr)
            {
                nativeThis.Handle = handle;
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromErrno(errno));
            }
        }

        if (nativeThis.Handle != nullptr)
        {
            auto& error = errno;

            error = 0;

            if (dirent* current = readdir(nativeThis.Handle))
            {
                std::string path{this->_root};
                Path::Push(path, current->d_name);

                return DirectoryEntry{
                    .Path = std::move(path),
                    .Type = Private::posix_into_FileType(*current),
                };
            }
            else if (error != 0)
            {
                return std::unexpected(Private::ErrorCodeFromErrno(errno));
            }
        }

        return std::nullopt;
    }

    std::string_view DirectoryEnumerator::Root() const
    {
        return this->_root;
    }
}

namespace Anemone::System
{
    std::expected<bool, ErrorCode> IsReadonly(const char* path)
    {
        if (access(path, W_OK) == 0)
        {
            return true;
        }

        auto error = errno;

        if (error == EACCES)
        {
            return false;
        }

        return std::unexpected(Private::ErrorCodeFromErrno(error));
    }

    std::expected<FileInfo, ErrorCode> FileInfo::FromPath(std::string_view path)
    {
        std::string spath{path};

        struct stat64 r;

        if (stat64(spath.c_str(), &r) == 0)
        {
            return FileInfo{
                .CreationTime = Platform::posix_into_DateTime(r.st_ctim),
                .LastAccessTime = Platform::posix_into_DateTime(r.st_atim),
                .LastWriteTime = Platform::posix_into_DateTime(r.st_mtim),
                .Size = static_cast<int64_t>(r.st_size),
                .Type = Private::posix_into_FileType(r),
                .Readonly = access(spath.c_str(), W_OK) != 0,
            };
        }

        return std::unexpected(Private::ErrorCodeFromErrno(errno));
    }

    std::expected<bool, ErrorCode> FileInfo::Exists(std::string_view path)
    {

        return access(std::string{path}.c_str(), F_OK) == 0;
    }

}
