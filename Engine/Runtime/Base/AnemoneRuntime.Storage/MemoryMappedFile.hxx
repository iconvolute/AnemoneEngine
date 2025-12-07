#pragma once
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.Storage/FileHandle.hxx"

namespace Anemone
{
    enum class MemoryMappedFileAccess : uint32_t
    {
        Read,
        ReadWrite,
        Write,
        ReadExecute,
        CopyOnWrite,
        ReadWriteExecute,
    };

    class MemoryMappedFileView;

    class ANEMONE_RUNTIME_BASE_API MemoryMappedFile : public ReferenceCounted<MemoryMappedFile>
    {
    public:
        MemoryMappedFile() = default;

        MemoryMappedFile(MemoryMappedFile const&) = delete;

        MemoryMappedFile(MemoryMappedFile&&) noexcept = delete;

        virtual ~MemoryMappedFile() = default;

        MemoryMappedFile& operator=(MemoryMappedFile const&) = delete;

        MemoryMappedFile& operator=(MemoryMappedFile&&) noexcept = delete;

    public:
        static Reference<MemoryMappedFile> Create(
            std::string_view path,
            FileMode mode,
            uint64_t capacity,
            MemoryMappedFileAccess access);

        virtual Reference<MemoryMappedFileView> CreateView(
            MemoryMappedFileAccess access,
            size_t offset = 0,
            size_t length = 0) = 0;

        virtual void Flush() = 0;
    };

    class ANEMONE_RUNTIME_BASE_API MemoryMappedFileView : public ReferenceCounted<MemoryMappedFileView>
    {
    public:
        MemoryMappedFileView() = default;

        MemoryMappedFileView(MemoryMappedFileView const&) = delete;

        MemoryMappedFileView(MemoryMappedFileView&&) noexcept = delete;

        virtual ~MemoryMappedFileView() = default;

        MemoryMappedFileView& operator=(MemoryMappedFileView const&) = delete;

        MemoryMappedFileView& operator=(MemoryMappedFileView&&) noexcept = delete;

    public:
        virtual std::span<std::byte const> GetData() const = 0;

        virtual std::span<std::byte> GetData() = 0;

        virtual void Flush() = 0;
    };
}
