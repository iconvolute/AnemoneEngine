#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#include <iterator>

namespace Anemone::Diagnostic
{
    struct DiagnosticState final
    {
        Threading::CriticalSection Lock;
        IntrusiveList<LogListener> Listeners;
    };

    static DiagnosticState GDiagnosticState{};

    RUNTIME_API bool AssertionFailed()
    {
        Threading::UniqueLock lock{GDiagnosticState.Lock};

        // report assertion to the user
        return true;
    }

    RUNTIME_API void AddLogListener(LogListener* listener)
    {
        Threading::UniqueLock lock{GDiagnosticState.Lock};
        GDiagnosticState.Listeners.PushBack(listener);
    }

    RUNTIME_API void RemoveLogListener(LogListener* listener)
    {
        Threading::UniqueLock lock{GDiagnosticState.Lock};
        GDiagnosticState.Listeners.Remove(listener);
    }

    RUNTIME_API void FlushLog()
    {
        Threading::UniqueLock lock{GDiagnosticState.Lock};

        GDiagnosticState.Listeners.ForEach([](LogListener& listener)
        {
            listener.Flush();
        });
    }

    RUNTIME_API void LogMessageCore(LogLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer;

        auto out = std::back_inserter(buffer);
        out = format_to(out, "[{}] ", std::to_string(std::to_underlying(level)));
        fmt::vformat_to(out, format, args);

        std::string_view message{buffer.data(), buffer.size()};

        Threading::UniqueLock lock{GDiagnosticState.Lock};
        GDiagnosticState.Listeners.ForEach([&](LogListener& listener)
        {
            listener.Log(level, message);
        });
    }
}
