#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <string_view>
#include <fmt/format.h>
#include <expected>

namespace Anemone
{
    class RUNTIME_API TextWriter : public ReferenceCounted<TextWriter>
    {
    public:
        TextWriter();

        TextWriter(TextWriter const&) = delete;

        TextWriter(TextWriter&&) = delete;

        virtual ~TextWriter();

        TextWriter& operator=(TextWriter const&) = delete;

        TextWriter& operator=(TextWriter&&) = delete;

    protected:
        virtual std::expected<void, Error> WriteCore(std::string_view value, bool newline) = 0;

    public:
        virtual std::expected<void, Error> Flush() = 0;

        std::expected<void, Error> Write(std::string_view value);

        template <typename... Args>
        std::expected<void, Error> Write(std::string_view format, Args const&... args);

        std::expected<void, Error> WriteFormatted(std::string_view format, fmt::format_args args);

        std::expected<void, Error> WriteLine();

        std::expected<void, Error> WriteLine(std::string_view value);

        template <typename... Args>
        std::expected<void, Error> WriteLine(std::string_view format, Args const&... args);

        std::expected<void, Error> WriteLineFormatted(std::string_view format, fmt::format_args args);
    };
}

namespace Anemone
{
    template <typename... Args>
    std::expected<void, Error> TextWriter::Write(std::string_view format, Args const&... args)
    {
        return this->WriteFormatted(
            format,
            fmt::make_format_args(args...));
    }

    template <typename... Args>
    std::expected<void, Error> TextWriter::WriteLine(std::string_view format, Args const&... args)
    {
        return this->WriteLineFormatted(
            format,
            fmt::make_format_args(args...));
    }
}
