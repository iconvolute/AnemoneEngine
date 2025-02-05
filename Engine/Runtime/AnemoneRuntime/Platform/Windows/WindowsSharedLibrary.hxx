#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <string_view>

namespace Anemone
{
    class WindowsSharedLibrary final
    {
    private:
        Interop::Win32SafeSharedLibraryHandle _handle;

    public:
        WindowsSharedLibrary() = default;

        explicit WindowsSharedLibrary(Interop::Win32SafeSharedLibraryHandle handle)
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

        [[nodiscard]] Interop::Win32SafeSharedLibraryHandle const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<WindowsSharedLibrary, ErrorCode> Open(
            std::string_view path);

        std::expected<void*, ErrorCode> GetSymbol(const char* name) const;
    };

    using SharedLibrary = WindowsSharedLibrary;
}
