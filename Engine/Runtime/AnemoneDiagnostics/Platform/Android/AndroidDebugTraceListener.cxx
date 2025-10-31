#include "AnemoneDiagnostics/Platform/Android/AndroidDebugTraceListener.hxx"
#include "AnemoneInterop/Windows/Debugger.hxx"

// 

namespace Anemone
{
    void AndroidDebugTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        (void)level;

        UniqueLock scope{this->m_lock};

        //__android_log_write(
        //    ANDROID_LOG_LEVEL,
        //)
    }
}
