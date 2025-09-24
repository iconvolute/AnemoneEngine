#include "AnemoneRuntime/System/Platform/Windows/WindowsClipboard.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsClipboard> gWindowsClipboard{};
    }

    void Clipboard::Initialize()
    {
        gWindowsClipboard.Create();

        gWindowsClipboard->binaryClipboardFormat = RegisterClipboardFormatW(L"AnemoneEngineBinaryData");
    }

    void Clipboard::Finalize()
    {
        gWindowsClipboard.Destroy();
    }

    auto Clipboard::Clear() -> std::expected<void, Error>
    {
        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();
            CloseClipboard();

            return {};
        }

        return std::unexpected(Error::NotReady);
    }

    auto Clipboard::GetText(std::string& result) -> std::expected<void, Error>
    {
        result.clear();

        if (OpenClipboard(nullptr))
        {
            if (HGLOBAL const handle = GetClipboardData(CF_UNICODETEXT))
            {
                if (wchar_t const* data = static_cast<wchar_t const*>(GlobalLock(handle)))
                {
                    Interop::Windows::NarrowString(result, data);

                    GlobalUnlock(handle);
                    CloseClipboard();

                    return {};
                }
            }

            if (HGLOBAL const handle = GetClipboardData(CF_TEXT))
            {
                if (char const* data = static_cast<char const*>(GlobalLock(handle)))
                {
                    result.assign(data);

                    GlobalUnlock(handle);
                    CloseClipboard();

                    return {};
                }
            }

            CloseClipboard();

            return std::unexpected(Error::InvalidFormat);
        }

        return std::unexpected(Error::NotReady);
    }

    auto Clipboard::SetText(std::string_view value) -> std::expected<void, Error>
    {
        using namespace Interop;
        using namespace Interop::Windows;

        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();

            string_buffer<wchar_t, 256> buffer{};
            WidenString(buffer, value);

            std::wstring_view const data = buffer.as_view();

            size_t const size = data.size() + 1; // Include null terminator.

            if (HGLOBAL const handle = GlobalAlloc(GMEM_MOVEABLE, size * sizeof(wchar_t)))
            {
                if (void* const destination = GlobalLock(handle))
                {
                    // Copy zero terminated string.
                    std::memcpy(destination, data.data(), size * sizeof(wchar_t));
                    GlobalUnlock(handle);

                    if (SetClipboardData(CF_UNICODETEXT, handle) != nullptr)
                    {
                        CloseClipboard();
                        return {};
                    }
                }
            }

            CloseClipboard();

            return std::unexpected(Error::NotEnoughMemory);
        }

        return std::unexpected(Error::NotReady);
    }
}
