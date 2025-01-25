#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>
#include <utility>

namespace Anemone::Diagnostics::Internal
{
    constexpr char GetCharacter(TraceLevel level)
    {
        switch (level)
        {
        case TraceLevel::Verbose:
            return 'V';

        case TraceLevel::Information:
            return 'I';

        case TraceLevel::Warning:
            return 'W';

        case TraceLevel::Error:
            return 'E';

        case TraceLevel::Critical:
            return 'C';

        case TraceLevel::Debug:
            return 'D';

        case TraceLevel::None:
            return 'N';
        }

        std::unreachable();
    }
}

namespace Anemone::Diagnostics
{
    UninitializedObject<Trace> GTrace{};

    Trace::Trace() = default;

    Trace::~Trace()
    {
        this->_listeners.Clear();
    }

    void Trace::AddListener(TraceListener& listener)
    {
        UniqueLock lock{this->_lock};

        this->_listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        UniqueLock lock{this->_lock};

        this->_listeners.Remove(&listener);
    }

    void Trace::WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
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

        SharedLock lock{this->_lock};

        this->_listeners.ForEach([&](TraceListener& listener)
        {
            listener.WriteLine(level, message, size);
        });
    }

    void Trace::Flush()
    {
        SharedLock lock{this->_lock};

        this->_listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }

}
