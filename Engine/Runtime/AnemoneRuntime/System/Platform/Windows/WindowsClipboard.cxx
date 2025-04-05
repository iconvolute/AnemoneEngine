#include "AnemoneRuntime/System/Platform/Windows/WindowsClipboard.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    UninitializedObject<WindowsClipboardStatics> GClipboardStatics{};

    WindowsClipboardStatics::WindowsClipboardStatics()
    {
        this->BinaryFormat = RegisterClipboardFormatW(L"AnemoneEngineBinaryData");
    }
}

namespace Anemone
{
    void Clipboard::Clear()
    {
        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();
            CloseClipboard();
        }
    }

    bool Clipboard::GetText(std::string& result)
    {
        result.clear();

        bool success = false;

        if (OpenClipboard(nullptr))
        {
            if (HGLOBAL const hUnicodeText = GetClipboardData(CF_UNICODETEXT); hUnicodeText != nullptr)
            {
                if (wchar_t const* const data = static_cast<wchar_t const*>(GlobalLock(hUnicodeText)))
                {
                    Interop::win32_NarrowString(result, data);
                    GlobalUnlock(hUnicodeText);
                    success = true;
                }
            }
            else if (HGLOBAL const hText = GetClipboardData(CF_TEXT); hText != nullptr)
            {
                if (char const* const data = static_cast<char const*>(GlobalLock(hText)))
                {
                    result = data;
                    GlobalUnlock(hText);
                    success = true;
                }
            }

            CloseClipboard();
        }

        return success;
    }

    bool Clipboard::SetText(std::string_view value)
    {
        bool success = false;

        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();

            Interop::string_buffer<wchar_t, 512> buffer{};
            Interop::win32_WidenString(buffer, value);
            std::wstring_view const data = buffer.as_view();

            if (HGLOBAL const handle = GlobalAlloc(GMEM_MOVEABLE, data.size() * sizeof(wchar_t)); handle != nullptr)
            {
                if (void* const destination = GlobalLock(handle); destination != nullptr)
                {
                    // Copy zero terminated string.
                    std::memcpy(destination, data.data(), data.size() * sizeof(wchar_t));
                    GlobalUnlock(handle);

                    if (SetClipboardData(CF_UNICODETEXT, handle) != nullptr)
                    {
                        success = true;
                    }
                }
            }

            CloseClipboard();
        }

        return success;
    }
}
