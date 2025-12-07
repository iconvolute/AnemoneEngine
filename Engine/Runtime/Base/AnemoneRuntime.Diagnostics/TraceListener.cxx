#include "AnemoneRuntime.Diagnostics/TraceListener.hxx"

#include <iterator>
#include <array>

namespace Anemone
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
    void TraceListener::TraceFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer{};

        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = GetCharacter(level);
        (*out++) = ']';
        (*out++) = ' ';
        out = fmt::vformat_to(out, format, args);

        size_t const size = buffer.size();

        (*out) = '\0';

        const char* message = buffer.data();

        this->TraceEvent(level, message, size);
    }
}
