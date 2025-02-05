#include "AnemoneRuntime/Platform/Windows/WindowsSharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    std::expected<WindowsSharedLibrary, ErrorCode> WindowsSharedLibrary::Open(
        std::string_view path)
    {
        Interop::win32_FilePathW wPath{};
        Interop::win32_WidenString(wPath, path);

        Interop::Win32SafeSharedLibraryHandle h{LoadLibraryW(wPath.c_str())};

        if (h)
        {
            return WindowsSharedLibrary{std::move(h)};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void*, ErrorCode> WindowsSharedLibrary::GetSymbol(const char* name) const
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            if (void* const symbol = GetProcAddress(this->_handle.Get(), name))
            {
                return symbol;
            }

            return std::unexpected(ErrorCode::NotFound);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
