#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

#include <iterator>

namespace Anemone::Diagnostic
{
    struct Logger final
    {
        Threading::CriticalSection Lock;
        IntrusiveList<LogListener> Listeners;
    };

    static Logger GLogger{};

    RUNTIME_API void AddLogListener(LogListener* listener)
    {
        Threading::UniqueLock lock{GLogger.Lock};
        GLogger.Listeners.PushBack(listener);
    }

    RUNTIME_API void RemoveLogListener(LogListener* listener)
    {
        Threading::UniqueLock lock{GLogger.Lock};
        GLogger.Listeners.Remove(listener);
    }

    RUNTIME_API void FlushLog()
    {
        Threading::UniqueLock lock{GLogger.Lock};

        GLogger.Listeners.ForEach([](LogListener& listener)
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

        Threading::UniqueLock lock{GLogger.Lock};
        GLogger.Listeners.ForEach([&](LogListener& listener)
        {
            listener.Log(level, message);
        });
    }
}
