#pragma once
#include "AnemoneRuntime/Storage/DataWriter.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"

namespace Anemone::Storage
{
    class RUNTIME_API FileHandleWriter final : public DataWriter
    {
    private:
        System::FileHandle _handle;
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        std::unique_ptr<std::byte[]> _buffer{};
        int64_t _file_position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    private:
        std::expected<void, ErrorCode> FlushBuffer();

    public:
        FileHandleWriter(Anemone::System::FileHandle handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleWriter(FileHandleWriter const&) = delete;
        FileHandleWriter(FileHandleWriter&&) = delete;
        FileHandleWriter& operator=(FileHandleWriter const&) = delete;
        FileHandleWriter& operator=(FileHandleWriter&&) = delete;

        ~FileHandleWriter() override;

        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> buffer) override;

        std::expected<void, ErrorCode> Flush() override;

        std::expected<void, ErrorCode> SetPosition(int64_t position) override;

        std::expected<int64_t, ErrorCode> GetPosition() const override;
    };
}
