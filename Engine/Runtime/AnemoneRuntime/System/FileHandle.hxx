#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/Flags.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"

#include <span>
#include <cstddef>
#include <expected>
#include <string_view>

namespace Anemone::System
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
        Inheritable = 1u << 2u,
        DeleteOnClose = 1u << 3u,
        RandomAccess = 1u << 4u,
        SequentialScan = 1u << 5u,
        WriteThrough = 1u << 6u,
        NoBuffering = 1u << 7u,
        ShareDelete = 1u << 8u,
    };

    class RUNTIME_API FileHandle final
    {
    private:
        Interop::NativeFileHandle m_native;

    public:
        explicit FileHandle(Interop::NativeFileHandle const& native);

        FileHandle();
        FileHandle(FileHandle const&) = delete;
        FileHandle(FileHandle&& other) noexcept;
        FileHandle& operator=(FileHandle const&) = delete;
        FileHandle& operator=(FileHandle&& other) noexcept;
        ~FileHandle();

    public:
        [[nodiscard]] static std::expected<FileHandle, ErrorCode> Create(std::string_view path, FileMode mode, Flags<FileAccess> access, Flags<FileOptions> options);

        [[nodiscard]] static std::expected<FileHandle, ErrorCode> Create(std::string_view path, FileMode mode, Flags<FileAccess> access)
        {
            return Create(path, mode, access, {});
        }

        [[nodiscard]] static std::expected<FileHandle, ErrorCode> Create(std::string_view path, FileMode mode)
        {
            return Create(path, mode, FileAccess::ReadWrite, {});
        }

        [[nodiscard]] static std::expected<void, ErrorCode> CreatePipe(FileHandle& read, FileHandle& write);

        [[nodiscard]] Interop::NativeFileHandle const& GetNative() const
        {
            return this->m_native;
        }

    public:
        std::expected<void, ErrorCode> Close();

        std::expected<void, ErrorCode> Flush();

        std::expected<int64_t, ErrorCode> GetLength() const;

        std::expected<void, ErrorCode> SetLength(int64_t length);

        std::expected<int64_t, ErrorCode> GetPosition() const;

        std::expected<void, ErrorCode> SetPosition(int64_t position);

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer, int64_t position);

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer, int64_t position);

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer);

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer);
    };

    // TODO: Implement Asynchronous File I/O
    // NOTE: This requires a tasking system to be implemented first, as we require ValueTask<T> to be implemented.
}
