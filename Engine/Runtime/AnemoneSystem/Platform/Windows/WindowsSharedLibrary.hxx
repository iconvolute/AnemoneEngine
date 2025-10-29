#pragma once
#include "AnemoneSystem/SharedLibrary.hxx"
#include "AnemoneInterop/Windows/SafeHandle.hxx"

#include <expected>

namespace Anemone
{
    class WindowsSharedLibrary final : public SharedLibrary
    {
    private:
        Interop::Windows::SafeSharedLibraryHandle _handle;

    protected:
        WindowsSharedLibrary();

    public:
        explicit WindowsSharedLibrary(Interop::Windows::SafeSharedLibraryHandle handle)
            : _handle{std::move(handle)}
        {
        }

        WindowsSharedLibrary(WindowsSharedLibrary const&) = delete;

        WindowsSharedLibrary(WindowsSharedLibrary&&) = delete;

        virtual ~WindowsSharedLibrary();

        WindowsSharedLibrary& operator=(WindowsSharedLibrary const&) = delete;

        WindowsSharedLibrary& operator=(WindowsSharedLibrary&&) = delete;

    public:
        static std::expected<Reference<WindowsSharedLibrary>, Error> Load(std::string_view path);

    public:
        std::expected<void*, Error> GetSymbol(const char* name) const override;
    };
}
