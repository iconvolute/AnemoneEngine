#include "AnemoneRuntime/System/Platform/Linux/LinuxClipboard.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<LinuxClipboard> gLinuxClipboard{};
    }

    void Clipboard::Initialize()
    {
        gLinuxClipboard.Create();
    }

    void Clipboard::Finalize()
    {
        gLinuxClipboard.Destroy();
    }

    auto Clipboard::Clear() -> std::expected<void, Error>
    {
        return std::unexpected(Error::NotImplemented);
    }

    auto Clipboard::GetText(std::string& result) -> std::expected<void, Error>
    {
        (void)result;
        return std::unexpected(Error::NotImplemented);
    }

    auto Clipboard::SetText(std::string_view value) -> std::expected<void, Error>
    {
        (void)value;
        return std::unexpected(Error::NotImplemented);
    }
}
