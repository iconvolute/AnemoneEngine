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
            if (this->_handle != nullptr)
            {
                if (not FreeLibrary(this->_handle))
                {
                    AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
                }
            }

            this->_handle = std::exchange(other._handle, nullptr);
        }

        return *this;
    }

    WindowsSharedLibrary::~WindowsSharedLibrary()
    {
        if (this->_handle != nullptr)
        {
            if (not FreeLibrary(this->_handle))
            {
                AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
            }
        }
    }

    std::expected<WindowsSharedLibrary, ErrorCode> WindowsSharedLibrary::Open(
        std::string_view path)
    {
        Interop::win32_FilePathW wPath{};
        Interop::win32_WidenString(wPath, path);

        if (HMODULE const h = LoadLibraryW(wPath.c_str()))
        {
            return WindowsSharedLibrary{h};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> WindowsSharedLibrary::Close()
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            if (not FreeLibrary(this->_handle))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void*, ErrorCode> WindowsSharedLibrary::GetSymbol(const char* name) const
    {
        AE_ASSERT(this->_handle);

        if (void* const symbol = GetProcAddress(this->_handle, name))
        {
            return symbol;
        }

        return std::unexpected(ErrorCode::NotFound);
    }
}
