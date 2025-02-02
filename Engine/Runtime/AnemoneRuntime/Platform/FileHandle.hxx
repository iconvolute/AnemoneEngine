#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidFileHandle.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxFileHandle.hxx"
#else
#error "Not Implemented"
#endif

#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <utility>
#include <string_view>
#include <expected>
#include <span>


namespace Anemone
{
    enum class FileMode : uint32_t
    {
        // Creates new file. Truncate if exists.
        CreateAlways,

        // Create new file. Fail if exists.
        CreateNew,

        // Create new file. Open if exists.
        OpenAlways,

        // Open if exists. Fail otherwise.
        OpenExisting,

        // Truncates existing file. Fails otherwise.
        TruncateExisting,
    };

    enum class FileAccess : uint32_t
    {
        Read = 1u << 0u,
        Write = 1u << 1u,
        ReadWrite = Read | Write,
    };

    enum class FileOptions : uint32_t
    {
        None = 0u,
        ShareRead = 1u << 0u,
        ShareWrite = 1u << 1u,
        Inheritable = 1u << 2u, // Is it really needed?
        DeleteOnClose = 1u << 3u,
        RandomAccess = 1u << 4u,
        SequentialScan = 1u << 5u,
        WriteThrough = 1u << 6u,
        NoBuffering = 1u << 7u,
        ShareDelete = 1u << 8u,
    };
}

namespace Anemone
{
    class RUNTIME_API FileHandle final
    {
    private:
        Internal::NativeFileHandle _handle = Internal::NativeFileHandle::Invalid();

    public:
        explicit FileHandle(Internal::NativeFileHandle handle);

        FileHandle() = default;

        FileHandle(FileHandle const&) = delete;

        FileHandle(FileHandle&& other) noexcept;

        FileHandle& operator=(FileHandle const&) = delete;

        FileHandle& operator=(FileHandle&& other) noexcept;

        ~FileHandle() noexcept;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::NativeFileHandle GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access,
            Flags<FileOptions> options);

        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode,
            Flags<FileAccess> access)
        {
            return Create(path, mode, access, FileOptions::None);
        }

        static std::expected<FileHandle, ErrorCode> Create(
            std::string_view path,
            FileMode mode)
        {
            return Create(path, mode, FileAccess::ReadWrite, FileOptions::None);
        }

        static std::expected<void, ErrorCode> CreatePipe(FileHandle& read, FileHandle& write);

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<void, ErrorCode> Flush();

        std::expected<int64_t, ErrorCode> GetLength() const;

        std::expected<void, ErrorCode> Truncate(int64_t length);

        std::expected<int64_t, ErrorCode> GetPosition() const;

        std::expected<void, ErrorCode> SetPosition(int64_t position);

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer);

        std::expected<size_t, ErrorCode> ReadAt(std::span<std::byte> buffer, int64_t position);

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer);

        std::expected<size_t, ErrorCode> WriteAt(std::span<std::byte const> buffer, int64_t position);
    };
}
