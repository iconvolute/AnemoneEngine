#pragma once
#include "AnemoneRuntime/Platform/Linux/LinuxSharedLibrary.hxx"
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
        void Close();

        void* GetSymbol(const char* name) const;

        explicit operator bool() const
        {
            return this->_handle != nullptr;
        }

        static std::expected<LinuxSharedLibrary, ErrorCode> Open(std::string_view path);
    };

    using SharedLibrary = LinuxSharedLibrary;
}
