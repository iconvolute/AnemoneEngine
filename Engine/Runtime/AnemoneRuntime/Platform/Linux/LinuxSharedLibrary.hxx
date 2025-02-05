#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

#include <expected>
#include <string_view>

namespace Anemone
{
    class LinuxSharedLibrary final
    {
    private:
        Interop::UnixSafeSharedLibraryHandle _handle;

    public:
        LinuxSharedLibrary() = default;

        explicit LinuxSharedLibrary(Interop::UnixSafeSharedLibraryHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Interop::UnixSafeSharedLibraryHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<LinuxSharedLibrary, ErrorCode> Open(
            std::string_view path);

        std::expected<void*, ErrorCode> GetSymbol(const char* name) const;
    };

    using SharedLibrary = LinuxSharedLibrary;
}
