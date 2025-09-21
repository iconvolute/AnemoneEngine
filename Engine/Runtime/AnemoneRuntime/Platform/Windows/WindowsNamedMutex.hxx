#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    class WindowsNamedMutex final
    {
    private:
        Interop::Windows::SafeHandle _handle;

    private:
        explicit WindowsNamedMutex(Interop::Windows::SafeHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        WindowsNamedMutex() = default;

    public:
        static std::expected<WindowsNamedMutex, Error> Create(std::string_view name);

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = WindowsNamedMutex;
}
