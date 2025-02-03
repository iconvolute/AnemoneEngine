#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsSharedLibrary.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxSharedLibrary.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidSharedLibrary.hxx"
#else
#error "Unsupported platform"
#endif

#include <expected>
#include <string_view>

namespace Anemone
{
    class SharedLibrary final
    {
    private:
        Internal::NativeSharedLibrary _handle;

    public:
        SharedLibrary() = default;

        SharedLibrary(SharedLibrary const&) = delete;

        SharedLibrary(SharedLibrary&& other) noexcept;

        SharedLibrary& operator=(SharedLibrary const&) = delete;

        SharedLibrary& operator=(SharedLibrary&& other) noexcept;

        ~SharedLibrary();

        explicit SharedLibrary(Internal::NativeSharedLibrary handle)
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

        [[nodiscard]] Internal::NativeSharedLibrary const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<SharedLibrary, ErrorCode> Open(
            std::string_view path);

        std::expected<void*, ErrorCode> GetSymbol(const char* name) const;
    };
}
