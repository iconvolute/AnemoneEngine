#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <expected>

namespace Anemone
{
    class WindowsNamedMutex final
    {
    private:
        Interop::Win32SafeHandle _handle;

    private:
        explicit WindowsNamedMutex(Interop::Win32SafeHandle handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        WindowsNamedMutex() = default;

    public:
        static std::expected<WindowsNamedMutex, ErrorCode> Create(std::string_view name);

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = WindowsNamedMutex;
}
