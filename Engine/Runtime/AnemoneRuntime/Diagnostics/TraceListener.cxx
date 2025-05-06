#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"

#include <iterator>
#include <array>

namespace Anemone::Diagnostics
{
    static constexpr std::array TraceLevelMarks = []()
    {
        return std::array{
            'V',
            'D',
            'I',
            'W',
            'E',
            'F',
            'N',
        };
    }();

    static constexpr char GetCharacter(TraceLevel level)
    {
        auto const index = std::to_underlying(level);
        if (index < TraceLevelMarks.size())
        {
            return TraceLevelMarks[index];
        }

        return 'N';
    }
    void TraceListener::LogEventFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer{};

        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = GetCharacter(level);
        (*out++) = ']';
        (*out++) = ' ';
        out = fmt::vformat_to(out, format, args);

        const char* message = buffer.data();
        size_t const size = buffer.size();

        (*out) = '\0';

        this->Event(level, message, size);
    }
}
