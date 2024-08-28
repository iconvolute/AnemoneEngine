#pragma once

#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeSocketStorage = UninitializedStorage<struct NativeSocket, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeSocketStorage = UninitializedStorage<struct NativeSocket, 8, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeSocketStorage = UninitializedStorage<struct NativeSocket, 8, 8>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Network
{
    class Socket
    {
    public:
        Platform::NativeSocketStorage Native;

    public:
        Socket(Platform::NativeSocket const& native);
        Socket();
        Socket(Socket const& other) = delete;
        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket const& other) = delete;
        Socket& operator=(Socket&& other) noexcept;
        ~Socket();
    };
}
