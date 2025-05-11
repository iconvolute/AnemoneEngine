#pragma once
#include "AnemoneRuntime/Storage/DataReader.hxx"

namespace Anemone::Storage
{
    class RUNTIME_API MemoryReader final : public DataReader
    {
    private:
        std::span<std::byte const> _buffer{};
        size_t _buffer_position;

    public:
        MemoryReader(std::span<std::byte const> buffer);
        MemoryReader(MemoryReader const&) = delete;
        MemoryReader(MemoryReader&&) = delete;
        MemoryReader& operator=(MemoryReader const&) = delete;
        MemoryReader& operator=(MemoryReader&&) = delete;
        ~MemoryReader() override;

    public:
        std::expected<size_t, Status> Read(std::span<std::byte> buffer) override;
        std::expected<void, Status> SetPosition(int64_t position) override;
        std::expected<int64_t, Status> GetPosition() const override;
    };
}
