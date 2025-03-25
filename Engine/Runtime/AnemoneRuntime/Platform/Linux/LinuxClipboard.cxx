#include "AnemoneRuntime/Platform/Clipboard.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    void Clipboard::Initialize()
    {
    }

    void Clipboard::Finalize()
    {
    }

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
