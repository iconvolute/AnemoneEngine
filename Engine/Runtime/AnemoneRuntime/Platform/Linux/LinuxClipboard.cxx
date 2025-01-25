#include "AnemoneRuntime/Platform/Linux/LinuxClipboard.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    void LinuxClipboard::Clear()
    {
        AE_PANIC("Not implemented");
    }

    bool LinuxClipboard::GetText(std::string& result)
    {
        (void)result;
        AE_PANIC("Not implemented");
        return false;
    }

    bool LinuxClipboard::SetText(std::string_view value)
    {
        (void)value;
        AE_PANIC("Not implemented");
        return false;
    }
}
