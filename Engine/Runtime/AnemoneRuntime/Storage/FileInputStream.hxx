#pragma once
#include "AnemoneRuntime/Storage/InputStream.hxx"
#include "AnemoneRuntime/Storage/FileHandle.hxx"

#include <memory>

namespace Anemone
{
    class RUNTIME_API FileInputStream : public InputStream
    {
    private:
        Reference<FileHandle> _handle{};
        size_t _bufferSize{};
        size_t _bufferCapacity{};
        size_t _bufferPosition{};
        std::unique_ptr<std::byte[]> _buffer{};
        uint64_t _filePosition{};
        uint64_t _fileSize{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

        std::expected<size_t, Error> ReadCore(
            std::span<std::byte> buffer,
            uint64_t position) const;

    public:
        explicit FileInputStream(Reference<FileHandle> handle);

        FileInputStream(Reference<FileHandle> handle, size_t bufferCapacity);

        FileInputStream(FileInputStream const&) = delete;

        FileInputStream(FileInputStream&&) = delete;

        ~FileInputStream() override;

        FileInputStream& operator=(FileInputStream const&) = delete;

        FileInputStream& operator=(
            FileInputStream&&) = delete;

    public:
        std::expected<size_t, Error> Read(std::span<std::byte> buffer) override;

        std::expected<void, Error> Skip(size_t count);

        std::expected<void, Error> SetPosition(uint64_t value) override;

        std::expected<uint64_t, Error> GetPosition() const override;

        std::expected<uint64_t, Error> GetLength() const override;
    };
}
