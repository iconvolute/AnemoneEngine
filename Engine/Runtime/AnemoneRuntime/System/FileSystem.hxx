#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"

#include <expected>
#include <span>
#include <cstddef>
#include <vector>

namespace Anemone::System
{
    enum class FileType
    {
        Unknown,
        Directory,
        File,
        SymbolicLink,
        BlockDevice,
        CharacterDevice,
        Socket,
        NamedPipe,
    };

    struct DirectoryEntry
    {
        std::string Path;
        FileType Type;
    };

    class RUNTIME_API DirectoryEnumerator
    {
    private:
        std::string m_root;
        Platform::NativeDirectoryEnumerator m_native;

    public:
        explicit DirectoryEnumerator(std::string root);
        DirectoryEnumerator(DirectoryEnumerator const&) = delete;
        DirectoryEnumerator(DirectoryEnumerator&& other) noexcept;
        DirectoryEnumerator& operator=(DirectoryEnumerator const&) = delete;
        DirectoryEnumerator& operator=(DirectoryEnumerator&& other) noexcept;
        ~DirectoryEnumerator();

    public:
        std::optional<std::expected<DirectoryEntry, ErrorCode>> Next();
        std::string_view Root() const;
    };

    RUNTIME_API std::expected<std::vector<DirectoryEntry>, ErrorCode> EnumerateDirectoryRecursively(std::string root, std::string_view pattern);

    struct FileInfo
    {
        DateTime CreationTime;
        DateTime LastAccessTime;
        DateTime LastWriteTime;
        int64_t Size;
        FileType Type;
        bool Readonly;

        RUNTIME_API static std::expected<FileInfo, ErrorCode> FromPath(std::string_view path);

        RUNTIME_API static std::expected<bool, ErrorCode> Exists(std::string_view path);
    };
}
