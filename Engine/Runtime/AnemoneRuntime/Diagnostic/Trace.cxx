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
        constexpr size_t DefaultBufferSize = 512;

        fmt::basic_memory_buffer<char, DefaultBufferSize> buffer;
        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = TraceLevelToCharacter(level);
        (*out++) = ']';
        out = fmt::vformat_to(out, format, args);

        std::string_view bufferView{buffer.data(), buffer.size()};

        // Ensure that the buffer is zero-terminated.
        (*out) = '\0';


        Threading::UniqueLock lock{this->m_lock};
        this->m_listeners.ForEach([&](TraceListener& listener)
        {
            listener.WriteLine(level, bufferView);
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
