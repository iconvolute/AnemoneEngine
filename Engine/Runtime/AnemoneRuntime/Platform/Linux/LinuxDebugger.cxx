#include "AnemoneRuntime/Platform/Debugger.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/ConsoleTraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <iterator>

namespace Anemone
{
    static UninitializedObject<ConsoleTraceListener> GConsoleTraceListener;

    void Debugger::Initialize()
    {
        GConsoleTraceListener.Create();
        Trace::AddListener(*GConsoleTraceListener);
    }

    void Debugger::Finalize()
    {
        Trace::RemoveListener(*GConsoleTraceListener);
        GConsoleTraceListener.Destroy();
    }

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
