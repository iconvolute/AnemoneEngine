#pragma once
#include "AnemoneRuntime/Storage/FileSystem.hxx"

namespace Anemone
{
    class LinuxFileHandle;

    class LinuxFileSystem : public FileSystem
    {
    public:
        LinuxFileSystem();
        
        LinuxFileSystem(LinuxFileSystem const&) = delete;
        
        LinuxFileSystem(LinuxFileSystem&&) = delete;
        
        LinuxFileSystem& operator=(LinuxFileSystem const&) = delete;
        
        LinuxFileSystem& operator=(LinuxFileSystem&&) = delete;
        
        ~LinuxFileSystem() override = default;

    public:
        bool CreatePipe(
            std::unique_ptr<FileHandle>& reader,
            std::unique_ptr<FileHandle>& writer) override;

        std::unique_ptr<FileHandle> CreateFileReader(std::string_view path) override;
        
        std::unique_ptr<FileHandle> CreateFileWriter(std::string_view path) override;

        bool FileExists(std::string_view path) override;
        
        bool FileDelete(std::string_view path) override;
        
        bool FileCopy(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) override;
        
        bool FileMove(std::string_view source, std::string_view destination, NameCollisionResolve nameCollisionResolve) override;
        
        std::optional<FileInfo> GetPathInfo(std::string_view path) override;
        
        bool DirectoryExists(std::string_view path) override;
        
        bool DirectoryDelete(std::string_view path) override;
        
        bool DirectoryDeleteRecursive(std::string_view path) override;
        
        bool DirectoryCreate(std::string_view path) override;
        
        bool DirectoryCreateRecursive(std::string_view path) override;
        
        bool DirectoryEnumerate(std::string_view path, FileSystemVisitor& visitor) override;
        
        bool DirectoryEnumerateRecursive(std::string_view path, FileSystemVisitor& visitor) override;
    };
}
