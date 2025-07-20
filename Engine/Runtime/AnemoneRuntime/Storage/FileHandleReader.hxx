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
        size_t _buffer_size{};
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        std::unique_ptr<std::byte[]> _buffer{};
        int64_t _file_position{};
        int64_t _file_size{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

        std::expected<size_t, Status> ReadCore(std::span<std::byte> buffer, int64_t position) const;

    public:
        FileHandleReader(std::unique_ptr<FileHandle> handle, size_t buffer_capacity = DefaultBufferCapacity);

        FileHandleReader(FileHandleReader const&) = delete;
        FileHandleReader(FileHandleReader&&) = delete;
        FileHandleReader& operator=(FileHandleReader const&) = delete;
        FileHandleReader& operator=(FileHandleReader&&) = delete;
        ~FileHandleReader() override = default;

        std::expected<size_t, Status> Read(std::span<std::byte> buffer) override;

        std::expected<void, Status> Skip(size_t count);

        std::expected<void, Status> SetPosition(int64_t position) override;

        std::expected<int64_t, Status> GetPosition() const override;
    };
}
