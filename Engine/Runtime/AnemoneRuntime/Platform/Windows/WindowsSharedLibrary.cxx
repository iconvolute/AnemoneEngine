#include "AnemoneRuntime/Platform/Windows/WindowsSharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{

    WindowsSharedLibrary::WindowsSharedLibrary(WindowsSharedLibrary&& other) noexcept
        : _handle{std::exchange(other._handle, nullptr)}
    {
    }

    WindowsSharedLibrary& WindowsSharedLibrary::operator=(WindowsSharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->Close();
            this->_handle = std::exchange(other._handle, nullptr);
        }

        return *this;
    }

    WindowsSharedLibrary::~WindowsSharedLibrary()
    {
        this->Close();
    }

    void WindowsSharedLibrary::Close()
    {
        if (this->_handle)
        {
            if (not FreeLibrary(this->_handle))
            {
                AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
            }

            this->_handle = nullptr;
        }
    }

    void* WindowsSharedLibrary::GetSymbol(const char* name) const
    {
        return std::bit_cast<void*>(GetProcAddress(this->_handle, name));
    }

    std::expected<WindowsSharedLibrary, ErrorCode> WindowsSharedLibrary::Open(std::string_view path)
    {
        Interop::win32_FilePathW wpath{};
        Interop::win32_WidenString(wpath, path);

        if (HMODULE h = LoadLibraryW(wpath.data()))
        {
            return WindowsSharedLibrary{h};
        }

        return std::unexpected(System::Private::ErrorCodeFromWin32Error(GetLastError()));
    }
}
