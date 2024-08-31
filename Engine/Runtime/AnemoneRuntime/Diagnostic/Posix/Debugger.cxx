#include "AnemoneRuntime/Diagnostic/Debugger.hxx"

#include <cstdlib>

namespace Anemone::Diagnostic
{
    void Abort(std::string_view message)
    {
        (void)message;
        abort();
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
