#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <iterator>
#include <utility>
#include <array>

namespace Anemone
{
    struct TraceStatics final
    {
        ReaderWriterLock Lock{};
        IntrusiveList<TraceListener, Trace> Listeners{};

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
    };

    static UninitializedObject<TraceStatics> GTraceStatics{};

    void Trace::Initialize()
    {
        GTraceStatics.Create();
    }

    void Trace::Finalize()
    {
        GTraceStatics.Destroy();
    }

    void Trace::AddListener(TraceListener& listener)
    {
        UniqueLock _{GTraceStatics->Lock};

        GTraceStatics->Listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        UniqueLock _{GTraceStatics->Lock};

        GTraceStatics->Listeners.Remove(&listener);
    }

    void Trace::TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        SharedLock _{GTraceStatics->Lock};

        if (not GTraceStatics->Listeners.IsEmpty())
        {
            fmt::memory_buffer buffer{};

            auto out = std::back_inserter(buffer);
            (*out++) = '[';
            (*out++) = TraceStatics::GetCharacter(level);
            (*out++) = ']';
            (*out++) = ' ';
            out = fmt::vformat_to(out, format, args);

            const char* message = buffer.data();
            size_t const size = buffer.size();

            (*out) = '\0';


            GTraceStatics->Listeners.ForEach([&](TraceListener& listener)
            {
                listener.TraceEvent(level, message, size);
            });
        }
    }

    void Trace::Flush()
    {
        SharedLock _{GTraceStatics->Lock};

        GTraceStatics->Listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }
}
