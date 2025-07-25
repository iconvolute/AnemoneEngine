#include "AnemoneRuntime/Storage/Platform/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Storage/Platform/Linux/FileHandle.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/FilePath.hxx"

#include "AnemoneRuntime/Interop/Linux/DateTime.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"

#include <dirent.h>
#include <string_view>
#include <unistd.h>
#include <sys/file.h>


namespace Anemone::Internal
{
    namespace
    {
        UninitializedObject<LinuxFileSystem> GPlatformFileSystem{};
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

        constexpr mode_t TranslateToOpenMode(Flags<FileOption> options)
        {
            mode_t result = S_IRUSR | S_IWUSR;

            if (options.Any(FileOption::ShareDelete))
            {
                result |= 0;
            }

            if (options.Any(FileOption::ShareRead))
            {
                result |= S_IRGRP | S_IROTH;
            }

            if (options.Any(FileOption::ShareWrite))
            {
                result |= S_IWGRP | S_IWOTH;
            }

            return result;
        }

        constexpr int TranslateToOpenFlags(FileMode mode, Flags<FileAccess> access, Flags<FileOption> options, bool failForSymlinks)
        {
            int result = O_CLOEXEC;

            if (failForSymlinks)
            {
                result |= O_NOFOLLOW;
            }

            switch (mode)
            {
            case FileMode::OpenExisting:
                break;

            case FileMode::TruncateExisting:
                result |= O_TRUNC;
                break;

            case FileMode::OpenAlways:
                result |= O_CREAT;
                break;

            case FileMode::CreateAlways:
                result |= O_CREAT | O_TRUNC;
                break;

            case FileMode::CreateNew:
                result |= O_CREAT | O_EXCL;
                break;
            }

            switch (access)
            {
            case FileAccess::Read:
                result |= O_RDONLY;
                break;

            case FileAccess::Write:
                result |= O_WRONLY;
                break;

            case FileAccess::ReadWrite:
                result |= O_RDWR;
                break;
            }

            if (options.Any(FileOption::WriteThrough))
            {
                result |= O_SYNC;
            }

            return result;
        }
    }

    LinuxFileSystem::LinuxFileSystem() = default;

    auto LinuxFileSystem::CreatePipe(
        std::unique_ptr<FileHandle>& outReader,
        std::unique_ptr<FileHandle>& outWriter)
        -> std::expected<void, ErrorCode>
    {
        using namespace Interop::Linux;

        int fd[2];

        if (pipe2(fd, O_CLOEXEC | O_NONBLOCK))
        {
            return std::unexpected(ErrorCode::Failure);
        }

        outReader = std::make_unique<LinuxFileHandle>(SafeFdHandle{fd[0]});
        outWriter = std::make_unique<LinuxFileHandle>(SafeFdHandle{fd[1]});

        return {};
    }

    auto LinuxFileSystem::CreateFile(
        std::string_view path,
        FileMode mode,
        Flags<FileAccess> access,
        Flags<FileOption> options)
        -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>
    {
        using namespace Interop::Linux;

        int const flags = TranslateToOpenFlags(mode, access, options, false);
        mode_t const fmode = TranslateToOpenMode(options);

        FilePath const filePath{path};

        SafeFdHandle handle{open(filePath.c_str(), flags, fmode)};

        if (handle)
        {
            if (flock(handle.Get(), LOCK_EX | LOCK_NB))
            {
                int const error = errno;

                if ((error == EAGAIN) or (error == EWOULDBLOCK))
                {
                    return std::unexpected(ErrorCode::Failure);
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(handle.Get(), 0))
                {
                    return std::unexpected(ErrorCode::Failure);
                }
            }

            return std::make_shared<LinuxFileHandle>(std::move(handle));
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::GetPathInfo(
        std::string_view path)
        -> std::expected<FileInfo, ErrorCode>
    {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::Exists(
        std::string_view path)
        -> std::expected<bool, ErrorCode>
    {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::FileDelete(
        std::string_view path)
        -> std::expected<void, ErrorCode>
    {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::FileMove(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryDelete(
        std::string_view path)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryDeleteRecursive(
        std::string_view path)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryCreate(
        std::string_view path)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryCreateRecursive(
        std::string_view path)
        -> std::expected<void, ErrorCode> {
        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryEnumerate(
        std::string_view path,
        FileSystemVisitor& visitor)
        -> std::expected<void, ErrorCode> {

        using namespace std::literals;
        using namespace Interop::Linux;

        std::string root{path};

        auto& error = errno;

        if (SafeDirHandle handle{opendir(root.c_str())})
        {
            while (true)
            {
                error = 0;

                dirent* current = readdir(handle.Get());

                if (current == nullptr)
                {
                    if (error != 0)
                    {
                        return std::unexpected(ErrorCode::Failure);
                    }

                    // End of directory.
                    return {};
                }

                FileType const fileType = FileTypeFromSystem(*current);
                std::string_view const name{current->d_name};

                if (fileType == FileType::Directory)
                {
                    if ((name == "."sv) || (name == ".."sv))
                    {
                        continue;
                    }
                }

                FilePath::PushFragment(root, name);

                FileInfo info{};
                struct stat64 s{};

                if (stat64(root.c_str(), &s) == 0)
                {
                    FileInfoFromSystem(s, info);
                }

                FilePath::PopFragment(root);

                visitor.Visit(root, name, info);
            }
        }

        return std::unexpected(ErrorCode::Failure);
    }

    auto LinuxFileSystem::DirectoryEnumerateRecursive(
        std::string_view path,
        FileSystemVisitor& visitor)
        -> std::expected<void, ErrorCode>
    {
        class RecursiveVisitor final : public FileSystemVisitor
        {
        private:
            LinuxFileSystem* _owner{};
            FileSystemVisitor* _visitor{};

        public:
            RecursiveVisitor(
                LinuxFileSystem* owner,
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
