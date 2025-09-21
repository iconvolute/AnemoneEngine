#pragma once
#include "AnemoneRuntime/Storage/MemoryMappedFile.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"

namespace Anemone
{
    class WindowsMemoryMappedFileView;

    class WindowsMemoryMappedFile final : public MemoryMappedFile
    {
        friend class WindowsMemoryMappedFileView;

    private:
        Interop::Windows::SafeFileHandle _fileHandle;
        Interop::Windows::SafeMemoryMappedFileHandle _mapHandle{};

    public:
        explicit WindowsMemoryMappedFile(Interop::Windows::SafeMemoryMappedFileHandle handle)
            : _mapHandle{std::move(handle)}
        {
            AE_ASSERT(this->_mapHandle);
        }

        explicit WindowsMemoryMappedFile(Interop::Windows::SafeFileHandle fileHandle, Interop::Windows::SafeMemoryMappedFileHandle mapHandle)
            : _fileHandle{std::move(fileHandle)}
            , _mapHandle{std::move(mapHandle)}
        {
            AE_ASSERT(this->_mapHandle);
            AE_ASSERT(this->_fileHandle);
        }

        ~WindowsMemoryMappedFile() override;

        static Reference<WindowsMemoryMappedFile> Create(std::string_view path, FileMode mode, uint64_t capacity, MemoryMappedFileAccess access);

        void Flush() override;

        Reference<MemoryMappedFileView> CreateView(MemoryMappedFileAccess access, size_t offset = 0, size_t length = 0) override;
    };

    class WindowsMemoryMappedFileView final : public MemoryMappedFileView
    {
        friend class WindowsMemoryMappedFile;

    public:
        WindowsMemoryMappedFileView(Reference<WindowsMemoryMappedFile> owner, Interop::Windows::SafeMemoryMappedViewHandle mapData, size_t offset, size_t size)
            : _owner{std::move(owner)}
            , _mapData{std::move(mapData)}
            , _offset{offset}
            , _size{size}
        {
        }

        WindowsMemoryMappedFileView(WindowsMemoryMappedFileView const&) = delete;

        WindowsMemoryMappedFileView(WindowsMemoryMappedFileView&&) noexcept = delete;

        ~WindowsMemoryMappedFileView() override = default;

        WindowsMemoryMappedFileView& operator=(WindowsMemoryMappedFileView const&) = delete;

        WindowsMemoryMappedFileView& operator=(WindowsMemoryMappedFileView&&) noexcept = delete;

        std::span<std::byte const> GetData() const override;

        std::span<std::byte> GetData() override;

        void Flush() override;

    private:
        Reference<WindowsMemoryMappedFile> _owner{};
        Interop::Windows::SafeMemoryMappedViewHandle _mapData{};
        size_t _offset;
        size_t _size;
    };
}
