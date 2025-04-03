#include "AnemoneRuntime/Diagnostics/Platform/Linux/LinuxDebugger.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Private/ConsoleTraceListener.hxx"

namespace Anemone::Private
{
    static UninitializedObject<ConsoleTraceListener> GConsoleTraceListener{};

    UninitializedObject<LinuxDebuggerStatics> GDebuggerStatics{};

    LinuxDebuggerStatics::LinuxDebuggerStatics()
    {
        GConsoleTraceListener.Create();
        Trace::AddListener(*GConsoleTraceListener);
    }

    LinuxDebuggerStatics::~LinuxDebuggerStatics()
    {
        Trace::RemoveListener(*GConsoleTraceListener);
        GConsoleTraceListener.Destroy();
    }
}

namespace Anemone
{
    void Debugger::Break()
    {
        anemone_debugbreak();
    }

    void Debugger::Crash()
    {
#if !ANEMONE_BUILD_SHIPPING
        anemone_debugbreak();
#endif

        abort();
    }

    bool Debugger::IsAttached()
    {
        return false;
    }

    void Debugger::Wait()
    {
    }

    bool Debugger::Attach()
    {
        return false;
    }

    void Debugger::ReportApplicationStop(std::string_view reason)
    {
        fwrite(reason.data(), 1, reason.size(), stderr);
        fflush(stderr);
        abort();
    }
}
