#pragma once
#include "AnemoneSystem/SharedLibrary.hxx"
#include "AnemoneInterop/Linux/SafeHandle.hxx"

#include <expected>

namespace Anemone
{
    class LinuxSharedLibrary final : public SharedLibrary
    {
    private:
        Interop::Linux::SafeSharedLibraryHandle _handle;

    protected:
        LinuxSharedLibrary();

    public:
        explicit LinuxSharedLibrary(Interop::Linux::SafeSharedLibraryHandle handle)
            : _handle{std::move(handle)}
        {
        }

        LinuxSharedLibrary(LinuxSharedLibrary const&) = delete;

        LinuxSharedLibrary(LinuxSharedLibrary&&) = delete;

        virtual ~LinuxSharedLibrary();

        LinuxSharedLibrary& operator=(LinuxSharedLibrary const&) = delete;

        LinuxSharedLibrary& operator=(LinuxSharedLibrary&&) = delete;

    public:
        static std::expected<Reference<LinuxSharedLibrary>, Error> Load(std::string_view path);

    public:
        std::expected<void*, Error> GetSymbol(const char* name) const override;
    };
}
