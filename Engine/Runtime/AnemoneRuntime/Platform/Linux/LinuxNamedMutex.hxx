#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

#include <string_view>
#include <semaphore.h>

namespace Anemone
{
    class LinuxNamedMutex final
    {
    private:
        sem_t* _handle{};

    public:
        explicit LinuxNamedMutex(std::string_view name);
        LinuxNamedMutex(LinuxNamedMutex const&) = delete;
        LinuxNamedMutex(LinuxNamedMutex&& other) noexcept;
        LinuxNamedMutex& operator=(LinuxNamedMutex const&) = delete;
        LinuxNamedMutex& operator=(LinuxNamedMutex&& other) noexcept;
        ~LinuxNamedMutex();

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };

    using NamedMutex = LinuxNamedMutex;
}
