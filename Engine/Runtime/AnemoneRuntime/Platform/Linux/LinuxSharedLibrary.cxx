#include "AnemoneRuntime/Platform/Linux/LinuxSharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <dlfcn.h>

namespace Anemone
{
    LinuxSharedLibrary::LinuxSharedLibrary(LinuxSharedLibrary&& other) noexcept
        : _handle{std::exchange(other._handle, nullptr)}
    {
    }

    LinuxSharedLibrary& LinuxSharedLibrary::operator=(LinuxSharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle)
            {
                if (dlclose(this->_handle))
                {
                    AE_TRACE(Error, "Failed to close shared library: handle={}, error={}",
                        fmt::ptr(this->_handle),
                        dlerror());
                }
            }

            this->_handle = std::exchange(other._handle, nullptr);
        }

        return *this;
    }

    LinuxSharedLibrary::~LinuxSharedLibrary()
    {
        if (this->_handle)
        {
            if (dlclose(this->_handle))
            {
                AE_TRACE(Error, "Failed to close shared library: handle={}, error={}",
                    fmt::ptr(this->_handle),
                    dlerror());
            }
        }
    }

    std::expected<LinuxSharedLibrary, ErrorCode> LinuxSharedLibrary::Open(
        std::string_view path)
    {
        Interop::unix_Path uPath{path};

        if (void* h = dlopen(uPath.c_str(), RTLD_LAZY))
        {
            return LinuxSharedLibrary{h};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> LinuxSharedLibrary::Close()
    {
        if (this->_handle)
        {
            if (dlclose(this->_handle))
            {
                return std::unexpected(ErrorCode::InvalidHandle);
            }
        }

        return {};
    }

    std::expected<void*, ErrorCode> LinuxSharedLibrary::GetSymbol(const char* name) const
    {
        if (void* symbol = dlsym(this->_handle, name))
        {
            return symbol;
        }

        return std::unexpected(ErrorCode::NotFound);
    }
}
