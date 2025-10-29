#pragma once
#include "AnemoneStorage/InputStream.hxx"
#include "AnemoneBase/MemoryBuffer.hxx"

namespace Anemone
{
    class RUNTIME_API MemoryInputStream final : public InputStream
    {
    private:
        Reference<MemoryBuffer> _buffer{};
        size_t _position;

    public:
        explicit MemoryInputStream(Reference<MemoryBuffer> buffer);

        MemoryInputStream(MemoryInputStream const&) = delete;

        MemoryInputStream(MemoryInputStream&&) = delete;

        ~MemoryInputStream() override;

        MemoryInputStream& operator=(MemoryInputStream const&) = delete;

        MemoryInputStream& operator=(MemoryInputStream&&) = delete;

    public:
        std::expected<size_t, Error> Read(std::span<std::byte> buffer) override;

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
