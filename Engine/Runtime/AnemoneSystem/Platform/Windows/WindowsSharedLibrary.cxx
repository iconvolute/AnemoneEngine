#include "AnemoneSystem/Platform/Windows/WindowsSharedLibrary.hxx"
#include "AnemoneInterop/Windows/FileSystem.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneDiagnostics/Platform/Windows/WindowsDebug.hxx"

#include <bit>

namespace Anemone
{
    WindowsSharedLibrary::WindowsSharedLibrary() = default;

    WindowsSharedLibrary::~WindowsSharedLibrary() = default;

    std::expected<Reference<WindowsSharedLibrary>, Error> WindowsSharedLibrary::Load(std::string_view path)
    {
        using namespace Interop::Windows;

        FilePathW wPath{};
        WidenString(wPath, path);

        if (SafeSharedLibraryHandle handle{LoadLibraryW(wPath.c_str())})
        {
            return MakeReference<WindowsSharedLibrary>(std::move(handle));
        }

        return std::unexpected(WindowsDebug::TranslateErrorCodeWin32(GetLastError()));
    }

    std::expected<void*, Error> WindowsSharedLibrary::GetSymbol(const char* name) const
    {
        AE_ASSERT(this->_handle);

        if (void* const symbol = std::bit_cast<void*>(GetProcAddress(this->_handle.Get(), name)))
        {
            return symbol;
        }

        return std::unexpected(WindowsDebug::TranslateErrorCodeWin32(GetLastError()));
    }
}

namespace Anemone
{
    std::expected<Reference<SharedLibrary>, Error> SharedLibrary::Load(std::string_view path)
    {
        return WindowsSharedLibrary::Load(path);
    }
}
