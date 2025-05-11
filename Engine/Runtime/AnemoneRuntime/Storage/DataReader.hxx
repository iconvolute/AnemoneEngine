#pragma once
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace Anemone::Storage
{
    class RUNTIME_API DataReader
    {
    public:
        DataReader() = default;
        DataReader(DataReader const&) = default;
        DataReader(DataReader&&) = default;
        DataReader& operator=(DataReader const&) = default;
        DataReader& operator=(DataReader&&) = default;
        virtual ~DataReader() = default;

    public:
        virtual std::expected<size_t, Status> Read(std::span<std::byte> buffer) = 0;

        virtual std::expected<void, Status> SetPosition(int64_t position) = 0;

        virtual std::expected<int64_t, Status> GetPosition() const = 0;
    };
}

namespace Anemone::Storage
{
    inline DataReader& operator>>(DataReader& reader, std::byte& value)
    {
        (void)reader.Read({&value, 1});
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, std::span<std::byte> buffer)
    {
        (void)reader.Read(buffer);
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, char& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, bool& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, signed char& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, signed short& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, signed int& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, signed long& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, signed long long& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, unsigned char& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, unsigned short& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, unsigned int& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, unsigned long& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, unsigned long long& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, float& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    inline DataReader& operator>>(DataReader& reader, double& value)
    {
        (void)reader.Read(std::as_writable_bytes(std::span{&value, 1}));
        return reader;
    }

    // size of the long double type is platform dependent
    DataReader& operator>>(DataReader& reader, long double& value) = delete;

    inline DataReader& operator>>(DataReader& reader, std::string& value)
    {
        uint64_t size{};
        reader >> size;
        value.resize(size);
        (void)reader.Read(std::as_writable_bytes(std::span{value.data(), size}));
        return reader;
    }
}
