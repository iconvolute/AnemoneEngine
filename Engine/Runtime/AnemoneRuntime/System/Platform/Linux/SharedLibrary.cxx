#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"

#include <utility>
#include <dlfcn.h>

namespace Anemone
{
    auto SharedLibrary::Load(std::string_view path) -> std::expected<SharedLibrary, Status>
    {
        Interop::Linux::FilePath uPath{path};

        Interop::Linux::SafeSharedLibraryHandle h{dlopen(uPath.c_str(), RTLD_LAZY)};

        if (h)
        {
            return SharedLibrary{std::move(h)};
        }

        return std::unexpected(Status::InvalidArgument);
    }

    auto SharedLibrary::GetSymbol(const char* name) const -> std::expected<void*, Status>
    {
        if (this->_handle)
        {
            if (void* symbol = dlsym(this->_handle.Get(), name))
            {
                return symbol;
            }

            return std::unexpected(Status::NotFound);
        }

        return std::unexpected(Status::InvalidHandle);
    }
}
