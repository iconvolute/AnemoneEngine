#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#include <utility>
#include <dlfcn.h>

namespace Anemone
{
    auto SharedLibrary::Load(std::string_view path) -> std::expected<SharedLibrary, ErrorCode>
    {
        Interop::unix_Path uPath{path};

        Interop::UnixSafeSharedLibraryHandle h{dlopen(uPath.c_str(), RTLD_LAZY)};

        if (h)
        {
            return LinuxSharedLibrary{std::move(h)};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    auto SharedLibrary::GetSymbol(const char* name) const -> std::expected<void*, ErrorCode>
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
