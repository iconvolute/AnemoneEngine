#include "AnemoneRuntime/Platform/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
    {
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary() = default;

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

    std::expected<void*, ErrorCode> SharedLibrary::GetSymbol(const char* name) const
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
