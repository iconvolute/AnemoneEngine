#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Internal
{
    extern void InitializeClipboard()
    {
    }

    extern void FinalizeClipboard()
    {
    }
}

namespace Anemone
{
    void Clipboard::Clear()
    {
        AE_PANIC("Not implemented");
    }

    bool Clipboard::GetText(std::string& result)
    {
        (void)result;
        AE_PANIC("Not implemented");
        return false;
    }

    bool Clipboard::SetText(std::string_view value)
    {
        (void)value;
        AE_PANIC("Not implemented");
        return false;
    }
}
