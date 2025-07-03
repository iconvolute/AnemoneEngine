#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    namespace
    {
        struct ClipboardStatics final
        {
            explicit ClipboardStatics()
            {
            }

            ClipboardStatics(ClipboardStatics const&) = delete;

            ClipboardStatics(ClipboardStatics&&) = delete;

            ClipboardStatics& operator=(ClipboardStatics const&) = delete;

            ClipboardStatics& operator=(ClipboardStatics&&) = delete;

            ~ClipboardStatics()
            {
            }
        };

        UninitializedObject<ClipboardStatics> gClipboardStatics{};
    }

    namespace Internal
    {
        extern void InitializeClipboard()
        {
            gClipboardStatics.Create();
        }

        extern void FinalizeClipboard()
        {
            gClipboardStatics.Destroy();
        }
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
