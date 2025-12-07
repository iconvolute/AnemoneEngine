#include "AnemoneRuntime.Diagnostics/Platform/Windows/WindowsDebugTraceListener.hxx"
#include "AnemoneRuntime.Interop/Windows/Debugger.hxx"

namespace Anemone
{
    void WindowsDebugTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        (void)level;

        UniqueLock scope{this->m_lock};
        Interop::Windows::OutputDebugStringLengthA(message, size + 1);
        Interop::Windows::OutputDebugStringLengthA("\r\n", 3);
    }
}
