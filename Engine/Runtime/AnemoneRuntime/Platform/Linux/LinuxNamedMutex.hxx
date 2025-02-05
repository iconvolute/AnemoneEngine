#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    class LinuxNamedMutex final
    {
    private:
        Interop::UnixSafeNamedSemaphoreHandle _handle;

    private:
        explicit LinuxNamedMutex(Interop::UnixSafeNamedSemaphoreHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        LinuxNamedMutex() = default;

    public:
        static std::expected<LinuxNamedMutex, ErrorCode> Create(std::string_view name);

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = LinuxNamedMutex;
}
