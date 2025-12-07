#pragma once
#include "AnemoneRuntime.Storage/OutputStream.hxx"
#include "AnemoneRuntime.Base/MemoryBuffer.hxx"

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API MemoryOutputStream final : public OutputStream
    {
    private:
        Reference<MemoryBuffer> _buffer;
        size_t _position{};

    public:
        explicit MemoryOutputStream(Reference<MemoryBuffer> buffer);

        MemoryOutputStream(MemoryOutputStream const&) = delete;

        MemoryOutputStream(MemoryOutputStream&&) = delete;

        ~MemoryOutputStream() override;

        MemoryOutputStream& operator=(MemoryOutputStream const&) = delete;

        MemoryOutputStream& operator=(MemoryOutputStream&&) = delete;

    public:
        std::expected<size_t, Error> Write(std::span<std::byte const> data) override;

        std::expected<void, Error> Flush() override;

        std::expected<void, Error> SetPosition(uint64_t position) override;

        std::expected<uint64_t, Error> GetPosition() const override;

        std::expected<uint64_t, Error> GetLength() const override;

    public:
        Reference<MemoryBuffer> GetBuffer() const
        {
            return this->_buffer;
        }
    };
}
