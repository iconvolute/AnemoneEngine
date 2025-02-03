#include "AnemoneRuntime/Platform/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <dlfcn.h>

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
        Interop::unix_Path uPath{path};

        if (void* h = dlopen(uPath.c_str(), RTLD_LAZY))
        {
            return SharedLibrary{Internal::NativeSharedLibrary{h}};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void*, ErrorCode> SharedLibrary::GetSymbol(const char* name) const
    {
        if (this->_handle)
        {
            if (void* symbol = dlsym(this->_handle.Get(), name))
            {
                return symbol;
            }

            return std::unexpected(ErrorCode::NotFound);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
