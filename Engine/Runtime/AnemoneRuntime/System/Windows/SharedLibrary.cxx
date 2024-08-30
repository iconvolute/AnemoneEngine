#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System
{
    SharedLibrary::SharedLibrary(Platform::NativeSharedLibrary const& native)
    {
        Platform::Create(this->_native, native);
    }

    SharedLibrary::SharedLibrary()
    {
        Platform::Create(this->_native);
    }

    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->Close();

            Platform::NativeSharedLibrary& nativeThis = Platform::Get(this->_native);
            Platform::NativeSharedLibrary& nativeOther = Platform::Get(other._native);

            nativeThis = std::exchange(nativeOther, {});
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary()
    {
        Platform::NativeSharedLibrary& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle)
        {
            if (not FreeLibrary(nativeThis.Handle))
            {
                AE_LOG(Error, "Failed to close shared library: {}\n", GetLastError());
            }
        }

        Platform::Destroy(this->_native);
    }

    std::expected<SharedLibrary, ErrorCode> SharedLibrary::Open(std::string_view path)
    {
        Platform::win32_FilePathW wpath{};
        Platform::win32_WidenString(wpath, path);

        if (HMODULE h = LoadLibraryW(wpath.data()))
        {
            return SharedLibrary{Platform::NativeSharedLibrary{h}};
        }

        return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
    }

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        Platform::NativeSharedLibrary& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle)
        {
            if (not FreeLibrary(nativeThis.Handle))
            {
                return std::unexpected(Private::ErrorCodeFromWin32Error(GetLastError()));
            }
        }

        return {};
    }

    SharedLibrary::operator bool() const
    {
        Platform::NativeSharedLibrary const& nativeThis = Platform::Get(this->_native);

        return nativeThis.Handle != nullptr;
    }

    void* SharedLibrary::GetSymbol(char const* name) const
    {
        Platform::NativeSharedLibrary const& nativeThis = Platform::Get(this->_native);

        return reinterpret_cast<void*>(GetProcAddress(nativeThis.Handle, name));
    }
}
