#include "AnemoneRuntime/System/Platform/Linux/LinuxClipboard.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<LinuxClipboard> gLinuxClipboard{};
    }

    void LinuxClipboard::Initialize()
    {
        gLinuxClipboard.Create();
    }

    void LinuxClipboard::Finalize()
    {
        gLinuxClipboard.Destroy();
    }

    LinuxClipboard& LinuxClipboard::Get()
    {
        return *gLinuxClipboard;
    }

    void Clipboard::Initialize()
    {
        LinuxClipboard::Initialize();
    }

    void Clipboard::Finalize()
    {
        LinuxClipboard::Finalize();
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
