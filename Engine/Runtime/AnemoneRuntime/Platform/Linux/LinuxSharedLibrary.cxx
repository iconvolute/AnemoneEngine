#include "AnemoneRuntime/Platform/Linux/LinuxSharedLibrary.hxx"

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
            this->Close();

            this->_handle = std::exchange(other._handle, nullptr);
        }

        return *this;
    }

    LinuxSharedLibrary::~LinuxSharedLibrary()
    {
        this->Close();
    }

    void LinuxSharedLibrary::Close()
    {
        if (this->_handle != nullptr)
        {
            dlclose(this->_handle);
            this->_handle = nullptr;
        }
    }

    void* LinuxSharedLibrary::GetSymbol(const char* name) const
    {
        return dlsym(this->_handle, name);
    }

    std::expected<LinuxSharedLibrary, ErrorCode> LinuxSharedLibrary::Open(std::string_view path)
    {
        if (void* h = dlopen(std::string{path}.c_str(), RTLD_LAZY))
        {
            return LinuxSharedLibrary{h};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }
}
