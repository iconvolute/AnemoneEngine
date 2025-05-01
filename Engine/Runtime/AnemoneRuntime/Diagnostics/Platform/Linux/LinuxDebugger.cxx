#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Linux/LinuxDebugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Internal/ConsoleTraceListener.hxx"

namespace Anemone::Internal
{
    static UninitializedObject<Diagnostics::ConsoleTraceListener> GConsoleTraceListener{};

    extern void InitializeDebugger()
    {
        GConsoleTraceListener.Create();
        Diagnostics::RegisterGlobalTraceListener(*GConsoleTraceListener);
    }

    extern void FinalizeDebugger()
    {
        Diagnostics::UnregisterGlobalTraceListener(*GConsoleTraceListener);
        GConsoleTraceListener.Destroy();
    }
}

namespace Anemone
{
    void Diagnostics::Break()
    {
        anemone_debugbreak();
    }

    void Diagnostics::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool Diagnostics::IsDebuggerAttached()
    {
        return false;
    }

    void Diagnostics::WaitForDebugger()
    {
    }

    bool Diagnostics::AttachDebugger()
    {
        return false;
    }

    void Diagnostics::ReportApplicationStop(std::string_view reason)
    {
        fwrite(reason.data(), 1, reason.size(), stderr);
        fflush(stderr);
        abort();
    }
}
