#include "AnemoneDiagnostics/ConsoleTraceListener.hxx"

namespace Anemone
{
    void ConsoleTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        (void)level;

        UniqueLock scope{this->_lock};

        std::fwrite(message, size, 1, stdout);
        std::fputc('\n', stdout);
    }

    void ConsoleTraceListener::Flush()
    {
        UniqueLock scope{this->_lock};

        std::fflush(stdout);
    }
}
