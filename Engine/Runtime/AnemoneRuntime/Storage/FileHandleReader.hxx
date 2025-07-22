#pragma once
#include "AnemoneRuntime/Storage/DataReader.hxx"
#include "AnemoneRuntime/Storage/FileHandle.hxx"

#include <memory>

namespace Anemone::Storage
{
    class RUNTIME_API FileHandleReader : public DataReader
    {
    private:
        std::unique_ptr<FileHandle> _handle{};
        size_t _bufferSize{};
        size_t _bufferCapacity{};
        size_t _bufferPosition{};
        std::unique_ptr<std::byte[]> _buffer{};
        uint64_t _filePosition{};
        uint64_t _fileSize{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

        size_t ReadCore(std::span<std::byte> buffer, uint64_t position) const;

    public:
        FileHandleReader(std::unique_ptr<FileHandle> handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleReader(FileHandleReader const&) = delete;
        FileHandleReader(FileHandleReader&&) = delete;
        FileHandleReader& operator=(FileHandleReader const&) = delete;
        FileHandleReader& operator=(FileHandleReader&&) = delete;
        ~FileHandleReader() override = default;

        size_t Read(std::span<std::byte> buffer) override;

        void Skip(size_t count);

        void SetPosition(uint64_t position) override;

        uint64_t GetPosition() const override;
    };
}
