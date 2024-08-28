#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <string>
#include <dlfcn.h>

namespace Anemone::System
{
    SharedLibrary::SharedLibrary(Platform::NativeSharedLibrary const& native)
    {
        Platform::Create(this->_native, native);
    }

    SharedLibrary::SharedLibrary()
    {
        Platform::Create(this->_native);
    }

    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != &other)
        {
            this->Close();

            Platform::NativeSharedLibrary& nativeThis = Platform::Get(this->_native);
            Platform::NativeSharedLibrary& nativeOther = Platform::Get(other._native);

            nativeThis = std::exchange(nativeOther, {});
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary()
    {
        this->Close();

        Platform::Destroy(this->_native);
    }

    std::expected<SharedLibrary, ErrorCode> SharedLibrary::Open(std::string_view path)
    {
        if (void* h = dlopen(std::string{path}.c_str(), RTLD_LAZY))
        {
            return SharedLibrary{
                Platform::NativeSharedLibrary{
                    .Handle = h
                }};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        Platform::NativeSharedLibrary& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle)
        {
            dlclose(nativeThis.Handle);
            return {};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    SharedLibrary::operator bool() const
    {
        Platform::NativeSharedLibrary const& nativeThis = Platform::Get(this->_native);

        return nativeThis.Handle != nullptr;
    }

    void* SharedLibrary::GetSymbol(char const* name) const
    {
        Platform::NativeSharedLibrary const& nativeThis = Platform::Get(this->_native);

        return dlsym(nativeThis.Handle, name);
    }
}
