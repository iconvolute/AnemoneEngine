#pragma once
#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/Math/Random.hxx"

namespace Anemone
{
    class WindowsFileHandle;

    class WindowsFileSystem : public FileSystem
    {
    public:
        WindowsFileSystem();
        
        WindowsFileSystem(WindowsFileSystem const&) = delete;
        
        WindowsFileSystem(WindowsFileSystem&&) = delete;
        
        WindowsFileSystem& operator=(WindowsFileSystem const&) = delete;
        
        WindowsFileSystem& operator=(WindowsFileSystem&&) = delete;
        
        ~WindowsFileSystem() override = default;

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

    private:
        // Random number generator for unique file names.
        Math::Xorshiro512ss _generator{};
    };
}
