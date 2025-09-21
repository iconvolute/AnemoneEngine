#pragma once
#include "AnemoneRuntime/Base/Flags.hxx"
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <expected>
#include <span>

namespace Anemone
{
    enum class FileMode : uint32_t
    {
        CreateNew,
        Create,
        Open,
        OpenOrCreate,
        Truncate,
        Append,
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
        Temporary = 1u << 9u,
    };

    class FileHandle : public ReferenceCounted<FileHandle>
    {
    public:
        FileHandle() = default;

        FileHandle(FileHandle const&) = delete;

        FileHandle(FileHandle&&) noexcept = delete;

        virtual ~FileHandle() = default;

        FileHandle& operator=(FileHandle const&) = delete;

        FileHandle& operator=(FileHandle&&) noexcept = delete;

    public:
        virtual std::expected<void, Error> Flush() = 0;

        virtual std::expected<uint64_t, Error> GetLength() const = 0;

        virtual std::expected<void, Error> SetLength(uint64_t length) = 0;

        virtual std::expected<uint64_t, Error> GetPosition() const = 0;

        virtual std::expected<void, Error> SetPosition(uint64_t position) = 0;

        virtual std::expected<size_t, Error> Read(std::span<std::byte> buffer) = 0;

        virtual std::expected<size_t, Error> ReadAt(std::span<std::byte> buffer, uint64_t position) = 0;

        virtual std::expected<size_t, Error> Write(std::span<std::byte const> buffer) = 0;

        virtual std::expected<size_t, Error> WriteAt(std::span<std::byte const> buffer, uint64_t position) = 0;
    };
}
