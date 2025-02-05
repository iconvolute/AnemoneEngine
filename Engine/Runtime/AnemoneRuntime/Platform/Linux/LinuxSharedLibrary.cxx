#include "AnemoneRuntime/Platform/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <dlfcn.h>

namespace Anemone
{
    std::expected<LinuxSharedLibrary, ErrorCode> LinuxSharedLibrary::Open(
        std::string_view path)
    {
        Interop::unix_Path uPath{path};

        Interop::UnixSafeSharedLibraryHandle h{dlopen(uPath.c_str(), RTLD_LAZY)};

        if (h)
        {
            return LinuxSharedLibrary{std::move(h)};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void*, ErrorCode> LinuxSharedLibrary::GetSymbol(const char* name) const
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
