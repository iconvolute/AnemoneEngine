#pragma once
#include "AnemoneRuntime/Storage/DataReader.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"

#include <memory>

namespace Anemone::Storage
{
    class RUNTIME_API FileHandleReader : public DataReader
    {
    private:
        System::FileHandle _handle{};
        size_t _buffer_size{};
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        std::unique_ptr<std::byte[]> _buffer{};
        int64_t _file_position{};
        int64_t _file_size{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

        std::expected<size_t, ErrorCode> ReadCore(std::span<std::byte> buffer, int64_t position);

    public:
        FileHandleReader(Anemone::System::FileHandle handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleReader(FileHandleReader const&) = delete;
        FileHandleReader(FileHandleReader&&) = delete;
        FileHandleReader& operator=(FileHandleReader const&) = delete;
        FileHandleReader& operator=(FileHandleReader&&) = delete;
        ~FileHandleReader() override = default;

        std::expected<size_t, ErrorCode> Read(std::span<std::byte> buffer) override;

        std::expected<void, ErrorCode> Skip(size_t count);

        std::expected<void, ErrorCode> SetPosition(int64_t position) override;

        std::expected<int64_t, ErrorCode> GetPosition() const override;
    };
}
