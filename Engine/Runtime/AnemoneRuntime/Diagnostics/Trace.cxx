#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/Diagnostics/Private/TraceStatics.hxx"

#include <iterator>
#include <utility>
#include <array>

namespace Anemone
{
    void Trace::AddListener(TraceListener& listener)
    {
        UniqueLock scope{Private::GTraceStatics->Lock};

        Private::GTraceStatics->Listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        UniqueLock scope{Private::GTraceStatics->Lock};

        Private::GTraceStatics->Listeners.Remove(&listener);
    }

    void Trace::TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        SharedLock scope{Private::GTraceStatics->Lock};

        if (not Private::GTraceStatics->Listeners.IsEmpty())
        {
            fmt::memory_buffer buffer{};

            auto out = std::back_inserter(buffer);
            (*out++) = '[';
            (*out++) = Private::TraceStatics::GetCharacter(level);
            (*out++) = ']';
            (*out++) = ' ';
            out = fmt::vformat_to(out, format, args);

            const char* message = buffer.data();
            size_t const size = buffer.size();

            (*out) = '\0';


            Private::GTraceStatics->Listeners.ForEach([&](TraceListener& listener)
            {
                listener.TraceEvent(level, message, size);
            });
        }
    }

    void Trace::Flush()
    {
        SharedLock scope{Private::GTraceStatics->Lock};

        Private::GTraceStatics->Listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }
}
