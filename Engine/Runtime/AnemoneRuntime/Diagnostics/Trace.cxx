#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>
#include <utility>

namespace Anemone::Internal
{
    constexpr char GetCharacter(TraceLevel level)
    {
        switch (level)
        {
        case TraceLevel::Verbose:
            return 'V';

        case TraceLevel::Debug:
            return 'D';

        case TraceLevel::Information:
            return 'I';

        case TraceLevel::Warning:
            return 'W';

        case TraceLevel::Error:
            return 'E';

        case TraceLevel::Fatal:
            return 'F';

        default:
        case TraceLevel::None:
            return 'N';
        }
    }

    struct TraceStatics final
    {
        ReaderWriterLock Lock{};
        IntrusiveList<TraceListener, Trace> Listeners{};
    };

    static TraceStatics GTraceStatics{};
}

namespace Anemone
{

    void Trace::AddListener(TraceListener& listener)
    {
        UniqueLock _{Internal::GTraceStatics.Lock};

        Internal::GTraceStatics.Listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        UniqueLock _{Internal::GTraceStatics.Lock};

        Internal::GTraceStatics.Listeners.Remove(&listener);
    }

    void Trace::TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        SharedLock _{Internal::GTraceStatics.Lock};

        if (not Internal::GTraceStatics.Listeners.IsEmpty())
        {
            fmt::memory_buffer buffer{};

            auto out = std::back_inserter(buffer);
            (*out++) = '[';
            (*out++) = Internal::GetCharacter(level);
            (*out++) = ']';
            (*out++) = ' ';
            out = fmt::vformat_to(out, format, args);

            const char* message = buffer.data();
            size_t const size = buffer.size();

            (*out) = '\0';


            Internal::GTraceStatics.Listeners.ForEach([&](TraceListener& listener)
            {
                listener.TraceEvent(level, message, size);
            });
        }
    }

    void Trace::Flush()
    {
        SharedLock _{Internal::GTraceStatics.Lock};

        Internal::GTraceStatics.Listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }
}
