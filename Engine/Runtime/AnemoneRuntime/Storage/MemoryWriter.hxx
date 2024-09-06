#pragma once
#include "AnemoneRuntime/Storage/DataWriter.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

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
        std::expected<size_t, ErrorCode> Write(std::span<std::byte const> data) override;

        std::expected<void, ErrorCode> Flush() override;

        std::expected<void, ErrorCode> SetPosition(int64_t position) override;

        std::expected<int64_t, ErrorCode> GetPosition() const override;
    };
}
