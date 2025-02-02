#include "AnemoneRuntime/Platform/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
        : _handle{std::exchange(other._handle, Internal::NativeSharedLibrary::Invalid())}
    {
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle.IsValid())
            {
                if (not FreeLibrary(this->_handle.Value))
                {
                    AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
                }
            }

            this->_handle = std::exchange(other._handle, Internal::NativeSharedLibrary::Invalid());
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary()
    {
        if (this->_handle.IsValid())
        {
            if (not FreeLibrary(this->_handle.Value))
            {
                AE_TRACE(Error, "Failed to close shared library: {}", GetLastError());
            }
        }
    }

    std::expected<SharedLibrary, ErrorCode> SharedLibrary::Open(
        std::string_view path)
    {
        Interop::win32_FilePathW wPath{};
        Interop::win32_WidenString(wPath, path);

        if (HMODULE const h = LoadLibraryW(wPath.c_str()))
        {
            return SharedLibrary{Internal::NativeSharedLibrary{h}};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        if (this->_handle.IsValid())
        {
            Internal::NativeSharedLibrary const handle = std::exchange(this->_handle, Internal::NativeSharedLibrary::Invalid());

            if (not FreeLibrary(handle.Value))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void*, ErrorCode> SharedLibrary::GetSymbol(const char* name) const
    {
        AE_ASSERT(this->_handle.IsValid());

        if (void* const symbol = GetProcAddress(this->_handle.Value, name))
        {
            return symbol;
        }

        return std::unexpected(ErrorCode::NotFound);
    }
}
