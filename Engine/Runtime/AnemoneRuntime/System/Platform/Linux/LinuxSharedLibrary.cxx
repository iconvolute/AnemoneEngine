#include "AnemoneRuntime/System/Platform/Linux/LinuxSharedLibrary.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Interop/Linux/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <utility>
#include <dlfcn.h>

namespace Anemone
{
    LinuxSharedLibrary::LinuxSharedLibrary() = default;

    LinuxSharedLibrary::~LinuxSharedLibrary() = default;

    std::expected<Reference<LinuxSharedLibrary>, Error> LinuxSharedLibrary::Load(std::string_view path)
    {
        using namespace Interop::Linux;

        FilePath uPath{path};

        if (SafeSharedLibraryHandle handle{dlopen(uPath.c_str(), RTLD_LAZY)})
        {
            return MakeReference<LinuxSharedLibrary>(std::move(handle));
        }

        return std::unexpected(Error::InvalidArgument);
    }

    std::expected<void*, Error> LinuxSharedLibrary::GetSymbol(const char* name) const
    {
        AE_ASSERT(this->_handle);

        if (void* symbol = dlsym(this->_handle.Get(), name))
        {
            return symbol;
        }

        return std::unexpected(Error::NotFound);
    }
}

namespace Anemone
{
    std::expected<Reference<SharedLibrary>, Error> SharedLibrary::Load(std::string_view path)
    {
        return LinuxSharedLibrary::Load(path);
    }
}
