#pragma once
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <string_view>

namespace Anemone::Storage
{
    class RUNTIME_API DataWriter
    {
    public:
        DataWriter() = default;
        DataWriter(const DataWriter&) = default;
        DataWriter(DataWriter&&) = default;
        DataWriter& operator=(const DataWriter&) = default;
        DataWriter& operator=(DataWriter&&) = default;
        virtual ~DataWriter() = default;

    public:
        [[nodiscard]] virtual std::expected<size_t, Status> Write(std::span<std::byte const> data) = 0;

        [[nodiscard]] virtual std::expected<void, Status> Flush() = 0;

        [[nodiscard]] virtual std::expected<void, Status> SetPosition(int64_t position) = 0;

        [[nodiscard]] virtual std::expected<int64_t, Status> GetPosition() const = 0;
    };
}

namespace Anemone::Storage
{
    inline DataWriter& operator<<(DataWriter& writer, std::byte const& value)
    {
        (void)writer.Write({&value, 1});
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, std::span<std::byte const> data)
    {
        (void)writer.Write(data);
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, char const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, bool const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, signed char const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, signed short const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, signed int const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, signed long const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, signed long long const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, unsigned char const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, unsigned short const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, unsigned int const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, unsigned long const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, unsigned long long const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, float const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, double const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{&value, 1}));
        return writer;
    }

    // size of the long double type is platform dependent
    DataWriter& operator<<(DataWriter& writer, long double const& value) = delete;

    inline DataWriter& operator<<(DataWriter& writer, std::string_view const& value)
    {
        (void)writer.Write(std::as_bytes(std::span{value.data(), value.size()}));
        return writer;
    }

    inline DataWriter& operator<<(DataWriter& writer, std::string const& value)
    {
        uint64_t size = value.size();
        writer << size;
        (void)writer.Write(std::as_bytes(std::span{value.data(), value.size()}));
        return writer;
    }
}
