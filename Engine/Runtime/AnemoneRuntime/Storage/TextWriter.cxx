#include "AnemoneRuntime/Storage/TextWriter.hxx"

namespace Anemone
{
    TextWriter::TextWriter() = default;

    TextWriter::~TextWriter() = default;

    std::expected<void, Error> TextWriter::Write(std::string_view value)
    {
        return this->WriteCore(value, false);
    }

    std::expected<void, Error> TextWriter::WriteFormatted(std::string_view format, fmt::format_args args)
    {
        return this->WriteCore(fmt::vformat(format, args), false);
    }

    std::expected<void, Error> TextWriter::WriteLine()
    {
        return this->WriteCore(std::string_view{}, true);
    }

    std::expected<void, Error> TextWriter::WriteLine(std::string_view value)
    {
        return this->WriteCore(value, true);
    }

    std::expected<void, Error> TextWriter::WriteLineFormatted(std::string_view format, fmt::format_args args)
    {
        return this->WriteCore(fmt::vformat(format, args), true);
    }
}
