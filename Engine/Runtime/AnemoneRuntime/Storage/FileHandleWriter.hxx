#pragma once
#include "AnemoneRuntime/Storage/DataWriter.hxx"
#include "AnemoneRuntime/Storage/FileHandle.hxx"

#include <memory>

namespace Anemone::Storage
{
    class RUNTIME_API FileHandleWriter final : public DataWriter
    {
    private:
        std::unique_ptr<FileHandle> _handle;
        size_t _bufferCapacity{};
        size_t _bufferPosition{};
        std::unique_ptr<std::byte[]> _buffer{};
        uint64_t _filePosition{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    private:
        void FlushBuffer();
        size_t WriteCore(std::span<std::byte const> buffer, uint64_t position) const;

    public:
        FileHandleWriter(std::unique_ptr<FileHandle> handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleWriter(FileHandleWriter const&) = delete;
        FileHandleWriter(FileHandleWriter&&) = delete;
        FileHandleWriter& operator=(FileHandleWriter const&) = delete;
        FileHandleWriter& operator=(FileHandleWriter&&) = delete;

        ~FileHandleWriter() override;

        size_t Write(std::span<std::byte const> buffer) override;

        void Flush() override;

        void SetPosition(uint64_t position) override;

        uint64_t GetPosition() const override;
    };
}
