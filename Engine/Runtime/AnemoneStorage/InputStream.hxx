#pragma once
#include "AnemoneBase/Reference.hxx"
#include "AnemoneDiagnostics/Error.hxx"

#include <expected>
#include <cstdint>
#include <span>

namespace Anemone
{
    class InputStream;
    using InputStreamRef = Reference<InputStream>;

    class RUNTIME_API InputStream : public ReferenceCounted<InputStream>
    {
    public:
        InputStream() = default;

        InputStream(InputStream const&) = delete;

        InputStream(InputStream&&) = delete;

        virtual ~InputStream() = default;

        InputStream& operator=(InputStream const&) = delete;

        InputStream& operator=(InputStream&&) = delete;

    public:
        virtual std::expected<size_t, Error> Read(std::span<std::byte> buffer) = 0;

        virtual std::expected<void, Error> SetPosition(uint64_t value) = 0;

        virtual std::expected<uint64_t, Error> GetPosition() const = 0;

        virtual std::expected<uint64_t, Error> GetLength() const = 0;
    };
}
