#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <string_view>

namespace Anemone
{
    class LinuxSharedLibrary final
    {
    private:
        void* _handle = nullptr;

    public:
        LinuxSharedLibrary() = default;

        LinuxSharedLibrary(LinuxSharedLibrary const&) = delete;

        LinuxSharedLibrary(LinuxSharedLibrary&& other) noexcept;

        LinuxSharedLibrary& operator=(LinuxSharedLibrary const&) = delete;

        LinuxSharedLibrary& operator=(LinuxSharedLibrary&& other) noexcept;

        ~LinuxSharedLibrary();

        explicit LinuxSharedLibrary(void* handle)
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

        [[nodiscard]] void* GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static std::expected<LinuxSharedLibrary, ErrorCode> Open(
            std::string_view path);

        std::expected<void, ErrorCode> Close();

        std::expected<void*, ErrorCode> GetSymbol(const char* name) const;
    };

    using SharedLibrary = LinuxSharedLibrary;
}
