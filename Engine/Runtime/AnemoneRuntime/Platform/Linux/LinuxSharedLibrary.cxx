#include "AnemoneRuntime/Platform/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <dlfcn.h>

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
                if (dlclose(this->_handle.Value))
                {
                    AE_TRACE(Error, "Failed to close shared library: handle={}, error={}",
                        fmt::ptr(this->_handle.Value),
                        dlerror());
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
            if (dlclose(this->_handle.Value))
            {
                AE_TRACE(Error, "Failed to close shared library: handle={}, error={}",
                    fmt::ptr(this->_handle.Value),
                    dlerror());
            }
        }
    }

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

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        if (this->_handle.IsValid())
        {
            Internal::NativeSharedLibrary handle = std::exchange(this->_handle, Internal::NativeSharedLibrary::Invalid());

            if (dlclose(handle.Value))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void*, ErrorCode> SharedLibrary::GetSymbol(const char* name) const
    {
        if (this->_handle.IsValid())
        {
            if (void* symbol = dlsym(this->_handle.Value, name))
            {
                return symbol;
            }

            return std::unexpected(ErrorCode::NotFound);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
