#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone
{
    struct TraceImpl
    {
        ReaderWriterLock Lock{};
        IntrusiveList<TraceListener, Trace> Listeners{};

        static constexpr char GetCharacter(TraceLevel level)
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

        static constexpr size_t DefaultBufferSize = 512;

        static TraceImpl& Get()
        {
            static TraceImpl instance{};
            return instance;
        }
    };

    void Trace::WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::basic_memory_buffer<char, TraceImpl::DefaultBufferSize> buffer{};
        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = TraceImpl::GetCharacter(level);
        (*out++) = ']';
        (*out++) = ' ';
        out = fmt::vformat_to(out, format, args);

        std::string_view view{buffer.data(), buffer.size()};
        (*out) = '\0';

        TraceImpl& self = TraceImpl::Get();

        UniqueLock _{self.Lock};

        self.Listeners.ForEach([&](TraceListener& listener)
        {
            listener.WriteLine(level, view);
        });
    }

    void Trace::Flush()
    {
        TraceImpl& self = TraceImpl::Get();
        UniqueLock _{self.Lock};

        self.Listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }

    void Trace::AddListener(TraceListener& listener)
    {
        TraceImpl& self = TraceImpl::Get();
        UniqueLock _{self.Lock};

        self.Listeners.PushBack(&listener);
    }

    void Trace::RemoveListener(TraceListener& listener)
    {
        TraceImpl& self = TraceImpl::Get();
        UniqueLock _{self.Lock};

        self.Listeners.Remove(&listener);
    }
};
