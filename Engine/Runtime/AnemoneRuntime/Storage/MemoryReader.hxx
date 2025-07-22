#pragma once
#include "AnemoneRuntime/Storage/DataReader.hxx"

namespace Anemone::Storage
{
    class RUNTIME_API MemoryReader final : public DataReader
    {
    private:
        std::span<std::byte const> _buffer{};
        size_t _position;

    public:
        MemoryReader(std::span<std::byte const> buffer);
        MemoryReader(MemoryReader const&) = delete;
        MemoryReader(MemoryReader&&) = delete;
        MemoryReader& operator=(MemoryReader const&) = delete;
        MemoryReader& operator=(MemoryReader&&) = delete;
        ~MemoryReader() override;

    public:
        size_t Read(std::span<std::byte> buffer) override;

        void SetPosition(uint64_t position) override;

        uint64_t GetPosition() const override;
    };
}
