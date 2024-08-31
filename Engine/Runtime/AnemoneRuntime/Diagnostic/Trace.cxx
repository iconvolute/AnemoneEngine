#include "AnemoneRuntime/Diagnostic/Trace.hxx"

#include <iterator>

namespace Anemone::Diagnostic
{
    constexpr char TraceLevelToCharacter(TraceLevel level)
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

    void Trace::WriteLineCore(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer;
        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = TraceLevelToCharacter(level);
        (*out++) = ']';
        out = fmt::vformat_to(out, format, args);
        (*out) = '\0';

        std::string_view message{buffer.data(), buffer.size() - 1};

        Threading::UniqueLock lock{this->m_lock};
        this->m_listeners.ForEach([&](TraceListener& listener)
        {
            listener.WriteLine(level, message);
        });
    }

    void Trace::Flush()
    {
        Threading::UniqueLock lock{this->m_lock};
        this->m_listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }

    void Trace::AddListener(TraceListener& listener)
    {
        Threading::UniqueLock lock{this->m_lock};
        this->m_listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        Threading::UniqueLock lock{this->m_lock};
        this->m_listeners.Remove(&listener);
    }

    UninitializedObject<Trace> GTrace{};
}
