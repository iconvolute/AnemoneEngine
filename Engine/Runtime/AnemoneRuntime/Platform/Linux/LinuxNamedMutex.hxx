#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    class LinuxNamedMutex final
    {
    private:
        Interop::Linux::SafeNamedSemaphoreHandle _handle;

    private:
        explicit LinuxNamedMutex(Interop::Linux::SafeNamedSemaphoreHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        LinuxNamedMutex() = default;

    public:
        static std::expected<LinuxNamedMutex, Error> Create(std::string_view name);

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = LinuxNamedMutex;
}
