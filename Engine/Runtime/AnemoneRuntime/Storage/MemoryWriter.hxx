#pragma once
#include "AnemoneRuntime/Storage/DataWriter.hxx"

#include <vector>

namespace Anemone::Storage
{
    class RUNTIME_API MemoryWriter final : public DataWriter
    {
    private:
        std::vector<std::byte>& _buffer;
        size_t _position{};

    public:
        explicit MemoryWriter(std::vector<std::byte>& buffer);

        MemoryWriter(MemoryWriter const&) = delete;
        MemoryWriter(MemoryWriter&&) = delete;
        MemoryWriter& operator=(MemoryWriter const&) = delete;
        MemoryWriter& operator=(MemoryWriter&&) = delete;
        ~MemoryWriter() override;

    public:
        size_t Write(std::span<std::byte const> data) override;

        void Flush() override;

        void SetPosition(uint64_t position) override;

        uint64_t GetPosition() const override;
    };
}
