#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <string_view>

namespace Anemone
{
    class WindowsSharedLibrary final
    {
    private:
        HMODULE _handle = nullptr;

    public:
        WindowsSharedLibrary() = default;

        WindowsSharedLibrary(WindowsSharedLibrary const&) = delete;

        WindowsSharedLibrary(WindowsSharedLibrary&& other) noexcept;

        WindowsSharedLibrary& operator=(WindowsSharedLibrary const&) = delete;

        WindowsSharedLibrary& operator=(WindowsSharedLibrary&& other) noexcept;

        ~WindowsSharedLibrary();

        explicit WindowsSharedLibrary(HMODULE handle)
            : _handle{handle}
        {
        }

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle != nullptr;
        }

        [[nodiscard]] HMODULE GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<WindowsSharedLibrary, ErrorCode> Open(
            std::string_view path);

        std::expected<void, ErrorCode> Close();

        std::expected<void*, ErrorCode> GetSymbol(const char* name) const;
    };

    using SharedLibrary = WindowsSharedLibrary;
}
