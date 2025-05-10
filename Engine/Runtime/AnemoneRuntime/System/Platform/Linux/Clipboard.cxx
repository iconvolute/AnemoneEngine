#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::Clipboard
{
    extern void Initialize()
    {
    }

    extern void Finalize()
    {
    }
}

namespace Anemone::Clipboard
{
    void Clear()
    {
        AE_PANIC("Not implemented");
    }

    bool GetText(std::string& result)
    {
        (void)result;
        AE_PANIC("Not implemented");
        return false;
    }

    bool SetText(std::string_view value)
    {
        (void)value;
        AE_PANIC("Not implemented");
        return false;
    }
}
