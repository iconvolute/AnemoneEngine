#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::System
{
    SharedLibrary::SharedLibrary(Interop::NativeSharedLibrary const& native)
        : m_native{native}
    {
    }

    SharedLibrary::SharedLibrary()
        : m_native{}
    {
    }

    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
        : m_native{std::exchange(other.m_native, {})}
    {
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->Close();

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary()
    {
        if (this->m_native.Handle)
        {
            if (not FreeLibrary(this->m_native.Handle))
            {
                AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
            }
        }
    }

    std::expected<SharedLibrary, ErrorCode> SharedLibrary::Open(std::string_view path)
    {
        Interop::win32_FilePathW wpath{};
        Interop::win32_WidenString(wpath, path);

        if (HMODULE h = LoadLibraryW(wpath.data()))
        {
            return SharedLibrary{Interop::NativeSharedLibrary{h}};
        }

        return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
    }

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        if (this->m_native.Handle)
        {
            if (not FreeLibrary(this->m_native.Handle))
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
            }

            this->m_native = {};
        }

        return {};
    }

    SharedLibrary::operator bool() const
    {
        return this->m_native.Handle != nullptr;
    }

    void* SharedLibrary::GetSymbol(char const* name) const
    {
        return reinterpret_cast<void*>(GetProcAddress(this->m_native.Handle, name));
    }
}
