#pragma once
#include "AnemoneRuntime/Platform/Base/BaseMemoryMappedFile.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"

#include <string_view>
#include <expected>
#include <span>

namespace Anemone
{
    class RUNTIME_API WindowsMemoryMappedFile final
    {
    private:
        Interop::Win32SafeMemoryMappedFileHandle _handle;

    private:
        WindowsMemoryMappedFile(Interop::Win32SafeMemoryMappedFileHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        WindowsMemoryMappedFile() = default;

        WindowsMemoryMappedFile(WindowsMemoryMappedFile const&) = delete;

        WindowsMemoryMappedFile(WindowsMemoryMappedFile&&) noexcept = default;

        WindowsMemoryMappedFile& operator=(WindowsMemoryMappedFile const&) = delete;

        WindowsMemoryMappedFile& operator=(WindowsMemoryMappedFile&&) noexcept = default;

        ~WindowsMemoryMappedFile() = default;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Interop::Win32SafeMemoryMappedFileHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    private:
        static auto OpenCore(
            const wchar_t* name,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        static auto CreateCore(
            Interop::Win32SafeFileHandle const& handle,
            const wchar_t* name,
            MemoryMappedFileAccess access,
            int64_t capacity)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        static auto OpenOrCreateCore(
            Interop::Win32SafeFileHandle const& handle,
            const wchar_t* name,
            MemoryMappedFileAccess access,
            int64_t capacity)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

    public:
        // Anonymous, in-process memory mapped file.
        static auto Create(
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        static auto Create(
            FileHandle const& fileHandle,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        // Named, create, fail if exists.
        static auto Create(
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        static auto Create(
            FileHandle const& fileHandle,
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        // Named, open, fail if doesn't exist.
        static auto Open(
            std::string_view name,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;

        // Named, create or open, fail for any other reason.
        static auto OpenOrCreate(
            FileHandle const& fileHandle,
            std::string_view name,
            int64_t capacity,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedFile, ErrorCode>;
    };

    using MemoryMappedFile = WindowsMemoryMappedFile;
}

namespace Anemone
{
    class RUNTIME_API WindowsMemoryMappedView final
    {
    private:
        Interop::Win32SafeMemoryMappedViewHandle _handle;

    private:
        WindowsMemoryMappedView(Interop::Win32SafeMemoryMappedViewHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        WindowsMemoryMappedView() = default;

        WindowsMemoryMappedView(WindowsMemoryMappedView const&) = delete;

        WindowsMemoryMappedView(WindowsMemoryMappedView&&) noexcept = default;

        WindowsMemoryMappedView& operator=(WindowsMemoryMappedView const&) = delete;

        WindowsMemoryMappedView& operator=(WindowsMemoryMappedView&&) noexcept = default;

        ~WindowsMemoryMappedView() = default;

    public:
        static auto Create(
            WindowsMemoryMappedFile const& fileHandle,
            MemoryMappedFileAccess access)
            -> std::expected<WindowsMemoryMappedView, ErrorCode>;

        static auto Create(
            WindowsMemoryMappedFile const& fileHandle,
            MemoryMappedFileAccess access,
            int64_t offset,
            int64_t size)
            -> std::expected<WindowsMemoryMappedView, ErrorCode>;

    public:
        std::expected<void, ErrorCode> Flush() const;

        std::expected<void, ErrorCode> Flush(
            size_t offset,
            size_t size) const;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }
        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }
        [[nodiscard]] Interop::Win32SafeMemoryMappedViewHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

        [[nodiscard]] void* GetData()
        {
            return this->_handle.GetData();
        }
        [[nodiscard]] void const* GetData() const
        {
            return this->_handle.GetData();
        }
        [[nodiscard]] size_t GetSize() const
        {
            return this->_handle.GetSize();
        }

        std::span<std::byte> AsSpan()
        {
            return std::span{static_cast<std::byte*>(this->GetData()), this->GetSize()};
        }

        std::span<std::byte const> AsSpan() const
        {
            return std::span{static_cast<std::byte const*>(this->GetData()), this->GetSize()};
        }
    };

    using MemoryMappedView = WindowsMemoryMappedView;
}
