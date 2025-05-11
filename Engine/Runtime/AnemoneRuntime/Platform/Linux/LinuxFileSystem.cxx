#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"

#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Error.hxx"

#include <dirent.h>

namespace Anemone::Internal
{
    constexpr FileType FileTypeFromSystem(dirent const& d)
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

    constexpr FileType FileTypeFromSystem(struct stat64 const& v)
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

    void FileInfoFromSystem(struct stat64 const& source, FileInfo& destination)
    {
        destination.Created = Interop::Linux::ToDateTime(source.st_ctim);
        destination.Accessed = Interop::Linux::ToDateTime(source.st_atim);
        destination.Modified = Interop::Linux::ToDateTime(source.st_mtim);
        destination.Size = static_cast<int64_t>(source.st_size);
        destination.Type = FileTypeFromSystem(source);
        destination.ReadOnly = (source.st_mode & S_IRUSR) == 0;
    }
}

namespace Anemone
{
    auto FileSystem::FileExists(std::string_view path) -> std::expected<bool, Status>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::FileDelete(std::string_view path) -> std::expected<void, Status>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
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
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve) -> std::expected<void, Status>
    {
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::GetFileInfo(std::string_view path) -> std::expected<FileInfo, Status>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::GetFileInfo(FileHandle const& handle) -> std::expected<FileInfo, Status>
    {
        (void)handle;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::DirectoryExists(std::string_view path) -> std::expected<bool, Status>
    {
        (void)path;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::DirectoryDelete(std::string_view path, bool recursive) -> std::expected<void, Status>
    {
        (void)path;
        (void)recursive;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::DirectoryCreate(std::string_view path, bool recursive) -> std::expected<void, Status>
    {
        (void)path;
        (void)recursive;
        AE_PANIC("Not implemented");
        return std::unexpected(Status::NotImplemented);
    }

    auto FileSystem::DirectoryEnumerate(std::string_view path, DirectoryVisitor& visitor) -> std::expected<void, Status>
    {
        std::string root{path};

        auto& error = errno;

        DIR* handle = opendir(root.c_str());

        if (handle == nullptr)
        {
            return std::unexpected(Internal::TranslateErrorCodeErrno(error));
        }

        while (true)
        {
            error = 0;

            if (dirent* current = readdir(handle))
            {
                FileType fileType = Internal::FileTypeFromSystem(*current);

                if (fileType == FileType::Directory)
                {
                    if ((current->d_name[0] == '.') and ((current->d_name[1] == '\0') or ((current->d_name[1] == '.') and (current->d_name[2] == '\0'))))
                    {
                        continue;
                    }
                }

                std::string child{root};
                FilePath::PushFragment(child, current->d_name);

                FileInfo info{};
                struct stat64 s{};

                if (stat64(child.c_str(), &s) == 0)
                {
                    Internal::FileInfoFromSystem(s, info);
                }

                visitor.Visit(child, info);
            }
            else if (error != 0)
            {
                return std::unexpected(Internal::TranslateErrorCodeErrno(error));
            }
            else
            {
                break;
            }
        }

        if (closedir(handle) != 0)
        {
            return std::unexpected(Internal::TranslateErrorCodeErrno(error));
        }

        return {};
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
                    FileSystem::DirectoryEnumerateRecursive(path, m_Visitor);
                }
            }
        };

        RecursiveVisitor recursiveVisitor{visitor};
        return FileSystem::DirectoryEnumerate(path, recursiveVisitor);
    }
}
