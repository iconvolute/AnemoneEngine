#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"

#include <string_view>
#include <string>

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeNamedMutexStorage = UninitializedStorage<struct NativeNamedMutex, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeNamedMutexStorage = UninitializedStorage<struct NativeNamedMutex, 8, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeNamedMutexStorage = UninitializedStorage<struct NativeNamedMutex, 8, 8>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::System
{
    class RUNTIME_API NamedMutex final
    {
    private:
        Platform::NativeNamedMutexStorage _native;
        std::string _name{};

    public:
        explicit NamedMutex(std::string_view name);
        NamedMutex(NamedMutex const&) = delete;
        NamedMutex(NamedMutex&& other) noexcept;
        NamedMutex& operator=(NamedMutex const&) = delete;
        NamedMutex& operator=(NamedMutex&& other) noexcept;
        ~NamedMutex();

        void Lock();
        bool TryLock();
        void Unlock();
    };
}
