#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

#include <string_view>

namespace Anemone
{
    class WindowsNamedMutex final
    {
    private:
        HANDLE _handle{};

    public:
        explicit WindowsNamedMutex(std::string_view name);
        WindowsNamedMutex(WindowsNamedMutex const&) = delete;
        WindowsNamedMutex(WindowsNamedMutex&& other) noexcept;
        WindowsNamedMutex& operator=(WindowsNamedMutex const&) = delete;
        WindowsNamedMutex& operator=(WindowsNamedMutex&& other) noexcept;
        ~WindowsNamedMutex();

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = WindowsNamedMutex;
}
