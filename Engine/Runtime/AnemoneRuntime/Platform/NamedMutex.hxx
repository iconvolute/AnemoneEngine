#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsNamedMutex.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxNamedMutex.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidNamedMutex.hxx"
#else
#error "Unsupported platform"
#endif

#include <string_view>

namespace Anemone
{
    class NamedMutex final
    {
    private:
        Internal::NativeNamedMutex _handle;

    public:
        explicit NamedMutex(std::string_view name);
        NamedMutex(NamedMutex const&) = delete;
        NamedMutex(NamedMutex&& other) noexcept;
        NamedMutex& operator=(NamedMutex const&) = delete;
        NamedMutex& operator=(NamedMutex&& other) noexcept;
        ~NamedMutex();

    public:
        void Lock();
        bool TryLock();
        void Unlock();
    };
}
