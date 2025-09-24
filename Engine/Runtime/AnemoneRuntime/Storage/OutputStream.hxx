#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <expected>
#include <cstdint>
#include <span>

namespace Anemone
{
    class OutputStream;
    using OutputStreamRef = Reference<OutputStream>;

    class RUNTIME_API OutputStream : public ReferenceCounted<OutputStream>
    {
    public:
        OutputStream() = default;

        OutputStream(OutputStream const&) = delete;

        OutputStream(OutputStream&&) = delete;

        virtual ~OutputStream() = default;

        OutputStream& operator=(OutputStream const&) = delete;

        OutputStream& operator=(OutputStream&&) = delete;

    public:
        virtual std::expected<size_t, Error> Write(std::span<std::byte const> buffer) = 0;

        virtual std::expected<void, Error> Flush() = 0;

        virtual std::expected<void, Error> SetPosition(uint64_t value) = 0;

        virtual std::expected<uint64_t, Error> GetPosition() const = 0;

        virtual std::expected<uint64_t, Error> GetLength() const = 0;
    };
}
