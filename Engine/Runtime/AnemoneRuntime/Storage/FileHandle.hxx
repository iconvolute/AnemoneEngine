#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/Flags.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

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

    enum class FileOption : uint32_t
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

    class RUNTIME_API FileHandle
    {
    public:
        FileHandle() = default;

        FileHandle(FileHandle const&) = delete;

        FileHandle(FileHandle&&) noexcept = default;

        FileHandle& operator=(FileHandle const&) = delete;

        FileHandle& operator=(FileHandle&&) noexcept = default;

        virtual ~FileHandle() = default;

    public:
        virtual void Flush() = 0;

        virtual uint64_t GetLength() const = 0;

        virtual void SetLength(uint64_t length) = 0;

        virtual uint64_t GetPosition() const = 0;

        virtual void SetPosition(uint64_t position) = 0;

        virtual size_t Read(std::span<std::byte> buffer) = 0;

        virtual size_t ReadAt(std::span<std::byte> buffer, uint64_t position) = 0;

        virtual size_t Write(std::span<std::byte const> buffer) = 0;

        virtual size_t WriteAt(std::span<std::byte const> buffer, uint64_t position) = 0;
    };
}
