#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <string>
#include <utility>
#include <dlfcn.h>

namespace Anemone::System
{
    SharedLibrary::SharedLibrary(Interop::NativeSharedLibrary const& native)
        : m_native{native}
    {
    }

    SharedLibrary::SharedLibrary()
        : m_native{}
    {
    }

    SharedLibrary::SharedLibrary(SharedLibrary&& other) noexcept
        : m_native{std::exchange(other.m_native, {})}
    {
    }

    SharedLibrary& SharedLibrary::operator=(SharedLibrary&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->Close();

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    SharedLibrary::~SharedLibrary()
    {
        this->Close();
    }

    std::expected<SharedLibrary, ErrorCode> SharedLibrary::Open(std::string_view path)
    {
        if (void* h = dlopen(std::string{path}.c_str(), RTLD_LAZY))
        {
            return SharedLibrary{
                Interop::NativeSharedLibrary{
                    .Handle = h,
                }};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    std::expected<void, ErrorCode> SharedLibrary::Close()
    {
        if (this->m_native.Handle)
        {
            dlclose(this->m_native.Handle);
            this->m_native.Handle = nullptr;
            return {};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    SharedLibrary::operator bool() const
    {
        return this->m_native.Handle != nullptr;
    }

    void* SharedLibrary::GetSymbol(char const* name) const
    {
        return dlsym(this->m_native.Handle, name);
    }
}
