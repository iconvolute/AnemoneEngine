#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone::Diagnostics::Private
{
    static constexpr size_t DefaultTraceBufferSize = 512;

    constexpr char GetCharacter(TraceLevel level)
    {
        switch (level)
        {
        case TraceLevel::Verbose:
            return 'V';

        case TraceLevel::Info:
            return 'I';

        case TraceLevel::Warning:
            return 'W';

        case TraceLevel::Error:
            return 'E';

        case TraceLevel::Critical:
            return 'C';

        case TraceLevel::None:
            return 'N';
        }

        std::unreachable();
    }
}

namespace Anemone::Diagnostics
{
    void Trace::WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::basic_memory_buffer<char, Private::DefaultTraceBufferSize> buffer{};
        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = Private::GetCharacter(level);
        (*out++) = ']';
        (*out++) = ' ';
        out = fmt::vformat_to(out, format, args);

        std::string_view view{buffer.data(), buffer.size()};
        (*out) = '\0';

        UniqueLock _{this->m_lock};

        this->m_listeners.ForEach([&](TraceListener& listener)
        {
            listener.WriteLine(level, view);
        });
    }

    
    void Trace::Flush()
    {
        UniqueLock _{this->m_lock};

        this->m_listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }

    void Trace::AddListener(TraceListener& listener)
    {
        UniqueLock _{this->m_lock};

        this->m_listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        UniqueLock _{this->m_lock};

        this->m_listeners.Remove(&listener);
    }

    UninitializedObject<Trace> GTrace;
}

anemone_noinline void XTrace::Formatted(Anemone::Diagnostics::TraceLevel level, std::string_view format, fmt::format_args args)
{
    ++this->m_count;
    (void)level;
    fmt::vprint(format, args);
}

Anemone::UninitializedObject<XTrace> GXtrace;
