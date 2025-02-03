#include "AnemoneRuntime/Diagnostics/ConsoleTraceListener.hxx"

namespace Anemone
{
    void ConsoleTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        (void)level;

        UniqueLock _{this->_lock};

        std::fwrite(message, size, 1, stdout);
        std::fputc('\n', stdout);
    }

    void ConsoleTraceListener::Flush()
    {
        UniqueLock _{this->_lock};

        std::fflush(stdout);
    }
}
