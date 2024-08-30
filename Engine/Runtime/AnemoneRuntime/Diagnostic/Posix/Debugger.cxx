#include "AnemoneRuntime/Diagnostic/Debugger.hxx"

namespace Anemone::Diagnostic
{
    void Abort(std::string_view message)
    {
        (void)message;
    }

    bool IsDebuggerAttached()
    {
        return false;
    }

    bool AttachDebugger()
    {
        return false;
    }
}
