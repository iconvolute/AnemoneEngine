#pragma once
#include "AnemoneRuntime/Platform/Base/BaseMemoryMappedFile.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <expected>
#include <span>

namespace Anemone
{
    class LinuxFileHandle;

    class RUNTIME_API LinuxMemoryMappedFile final
    {
    public:
        LinuxMemoryMappedFile() = default;

        LinuxMemoryMappedFile(LinuxMemoryMappedFile const&) = delete;

        LinuxMemoryMappedFile(LinuxMemoryMappedFile&&) noexcept = default;

        LinuxMemoryMappedFile& operator=(LinuxMemoryMappedFile const&) = delete;

        LinuxMemoryMappedFile& operator=(LinuxMemoryMappedFile&&) noexcept = default;

        ~LinuxMemoryMappedFile() = default;

    public:
        static auto Create(
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>
        {
            (void)capacity;
            (void)access;
            return std::unexpected(ErrorCode::NotImplemented);
        }

        static auto Create(
            LinuxFileHandle const& fileHandle,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>
        {
            (void)fileHandle;
            (void)capacity;
            (void)access;
            return std::unexpected(ErrorCode::NotImplemented);
        }

        static auto Create(
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>
            {
                (void)name;
                (void)capacity;
                (void)access;
                return std::unexpected(ErrorCode::NotImplemented);
            }

        static auto Create(
            LinuxFileHandle const& fileHandle,
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>{
                (void)fileHandle;
                (void)name;
                (void)capacity;
                (void)access;
                return std::unexpected(ErrorCode::NotImplemented);
            }

        static auto Open(
            std::string_view name,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>
        {
            (void)name;
            (void)access;
            return std::unexpected(ErrorCode::NotImplemented);
        }

        static auto OpenOrCreate(
            LinuxFileHandle const& fileHandle,
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedFile, ErrorCode>
        {
            (void)fileHandle;
            (void)name;
            (void)capacity;
            (void)access;
            return std::unexpected(ErrorCode::NotImplemented);
        }
    };

    using MemoryMappedFile = LinuxMemoryMappedFile;
}

namespace Anemone
{
    class RUNTIME_API LinuxMemoryMappedView final
    {
    public:
        LinuxMemoryMappedView() = default;

        LinuxMemoryMappedView(LinuxMemoryMappedView const&) = delete;

        LinuxMemoryMappedView(LinuxMemoryMappedView&&) noexcept = default;

        LinuxMemoryMappedView& operator=(LinuxMemoryMappedView const&) = delete;

        LinuxMemoryMappedView& operator=(LinuxMemoryMappedView&&) noexcept = default;

        ~LinuxMemoryMappedView() = default;

    public:
        static auto Create(
            LinuxMemoryMappedFile const& fileHandle,
            MemoryMappedFileAccess access)
            -> std::expected<LinuxMemoryMappedView, ErrorCode>
        {
            (void)fileHandle;
            (void)access;
            return std::unexpected(ErrorCode::NotImplemented);
        }

        static auto Create(
            LinuxMemoryMappedFile const& fileHandle,
            MemoryMappedFileAccess access,
            int64_t offset,
            int64_t size)
            -> std::expected<LinuxMemoryMappedView, ErrorCode>
        {
            (void)fileHandle;
            (void)access;
            (void)offset;
            (void)size;
            return std::unexpected(ErrorCode::NotImplemented);
        }

    public:
        std::expected<void, ErrorCode> Flush() const
        {
            return std::unexpected(ErrorCode::NotImplemented);
        }

        std::expected<void, ErrorCode> Flush(
            size_t offset,
            size_t size) const
        {
            (void)offset;
            (void)size;
            return std::unexpected(ErrorCode::NotImplemented);
        }
    };

    using MemoryMappedView = LinuxMemoryMappedView;
}
