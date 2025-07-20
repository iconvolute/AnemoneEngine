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
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        std::unique_ptr<std::byte[]> _buffer{};
        int64_t _file_position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    private:
        std::expected<void, Status> FlushBuffer();
        std::expected<size_t, Status> WriteCore(std::span<std::byte const> buffer, int64_t position) const;

    public:
        FileHandleWriter(std::unique_ptr<FileHandle> handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleWriter(FileHandleWriter const&) = delete;
        FileHandleWriter(FileHandleWriter&&) = delete;
        FileHandleWriter& operator=(FileHandleWriter const&) = delete;
        FileHandleWriter& operator=(FileHandleWriter&&) = delete;

        ~FileHandleWriter() override;

        std::expected<size_t, Status> Write(std::span<std::byte const> buffer) override;

        std::expected<void, Status> Flush() override;

        std::expected<void, Status> SetPosition(int64_t position) override;

        std::expected<int64_t, Status> GetPosition() const override;
    };
}
