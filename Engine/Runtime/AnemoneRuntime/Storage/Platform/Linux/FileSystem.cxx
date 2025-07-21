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


namespace Anemone::Internal
{
    UninitializedObject<LinuxFileSystem> GPlatformFileSystem{};

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
    }

    LinuxFileSystem::LinuxFileSystem()
    {
    }

    bool LinuxFileSystem::CreatePipe(std::unique_ptr<FileHandle>& reader, std::unique_ptr<FileHandle>& writer)
    {
        reader = nullptr;
        writer = nullptr;
        return false;
    }

    auto LinuxFileSystem::CreateFileReader(std::string_view path) -> std::unique_ptr<FileHandle>
    {
        (void)path;
        return nullptr;
    }

    auto LinuxFileSystem::CreateFileWriter(std::string_view path) -> std::unique_ptr<FileHandle>
    {
        (void)path;
        return nullptr;
    }

    bool LinuxFileSystem::FileExists(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::FileDelete(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::FileCopy(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve)
    {
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        return false;
    }

    bool LinuxFileSystem::FileMove(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve)
    {
        (void)source;
        (void)destination;
        (void)nameCollisionResolve;
        return false;
    }

    auto LinuxFileSystem::GetPathInfo(std::string_view path) -> std::optional<FileInfo>
    {
        std::string root{path};
        return {};
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


    bool LinuxFileSystem::DirectoryExists(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::DirectoryDelete(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::DirectoryDeleteRecursive(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::DirectoryCreate(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::DirectoryCreateRecursive(std::string_view path)
    {
        (void)path;
        return false;
    }

    bool LinuxFileSystem::DirectoryEnumerate(std::string_view path, FileSystemVisitor& visitor)
    {
        using namespace std::literals;

        std::string root{path};

        auto& error = errno;

        if (Interop::Linux::SafeDirHandle handle{ opendir(root.c_str()) })
        {
            while (true)
            {
                error = 0;
                
                dirent* current = readdir(handle.Get());

                if (current == nullptr)
                {
                    if (error != 0)
                    {
                        AE_TRACE(Error, "Failed to read directory '{}': {}", root, Diagnostics::Debug::TranslateErrorCodeErrno(error));
                        return false;
                    }

                    return true;
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

        return false;
    }

    bool LinuxFileSystem::DirectoryEnumerateRecursive(std::string_view path, FileSystemVisitor& visitor)
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
