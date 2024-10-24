#include "AnemoneRuntime/System/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/System/Path.hxx"

#include <string>
#include <utility>

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
        : m_root{std::move(root)}
        , m_native{}
    {
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other) noexcept
        : m_root{std::exchange(other.m_root, {})}
        , m_native{std::exchange(other.m_native, {})}
    {
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->m_native.Handle != nullptr)
            {
                if (closedir(this->m_native.Handle) != 0)
                {
                    AE_PANIC("Failed to close directory enumerator (errno = {})", errno);
                }
            }

            this->m_root = std::exchange(other.m_root, {});
            this->m_native = std::exchange(this->m_native, Platform::NativeDirectoryEnumerator{nullptr});
        }

        return *this;
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        if (this->m_native.Handle != nullptr)
        {
            if (closedir(this->m_native.Handle) != 0)
            {
                AE_PANIC("Failed to close directory enumerator (errno = {})", errno);
            }
        }
    }

    std::optional<std::expected<DirectoryEntry, ErrorCode>> DirectoryEnumerator::Next()
    {
        if (this->m_native.Handle == nullptr)
        {
            DIR* handle = opendir(this->m_root.c_str());

            if (handle != nullptr)
            {
                this->m_native.Handle = handle;
            }
            else
            {
                return std::unexpected(Private::ErrorCodeFromErrno(errno));
            }
        }

        if (this->m_native.Handle != nullptr)
        {
            auto& error = errno;

            error = 0;

            if (dirent* current = readdir(this->m_native.Handle))
            {
                std::string path{this->m_root};
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
        return this->m_root;
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
