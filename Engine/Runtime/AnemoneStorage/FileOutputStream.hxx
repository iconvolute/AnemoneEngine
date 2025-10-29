#pragma once
#include "AnemoneStorage/OutputStream.hxx"
#include "AnemoneStorage/FileHandle.hxx"

#include <memory>

namespace Anemone
{
    class RUNTIME_API FileOutputStream : public OutputStream
    {
    private:
        Reference<FileHandle> _handle{};
        size_t _bufferCapacity{};
        size_t _bufferPosition{};
        std::unique_ptr<std::byte[]> _buffer{};
        uint64_t _filePosition{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    private:
        std::expected<void, Error> FlushBuffer();

        std::expected<size_t, Error> WriteCore(std::span<std::byte const> buffer, uint64_t position) const;

    public:
        explicit FileOutputStream(Reference<FileHandle> handle);

        FileOutputStream(Reference<FileHandle> handle, size_t bufferCapacity);

        FileOutputStream(FileOutputStream const&) = delete;

        FileOutputStream(FileOutputStream&&) = delete;

        ~FileOutputStream() override;

        FileOutputStream& operator=(FileOutputStream const&) = delete;

        FileOutputStream& operator=(FileOutputStream&&) = delete;

    public:
        std::expected<size_t, Error> Write(std::span<std::byte const> buffer) override;

        std::expected<void, Error> Flush() override;

        std::expected<void, Error> SetPosition(uint64_t value) override;

        std::expected<uint64_t, Error> GetPosition() const override;

        std::expected<uint64_t, Error> GetLength() const override;
    };
}
