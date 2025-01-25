#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <dirent.h>

#include <atomic>

namespace Anemone::Interop
{
    struct NativeIpEndPoint final
    {
        union
        {
            sockaddr Header;
            sockaddr_in V4;
            sockaddr_in6 V6;
        } Address;
    };

    struct NativeLocalEndPoint final
    {
        sockaddr_un Inner;
    };

    struct NativeSocket final
    {
        int Inner;
    };

    struct NativeSharedLibrary final
    {
        void* Handle;
    };

    struct NativeProcess final
    {
        pid_t Pid;
    };

    struct NativeNamedMutex final
    {
        sem_t* Handle;
    };

    struct NativeFileHandle final
    {
        int Descriptor;
    };

    struct NativeAsyncFileHandle final
    {
        int Descriptor;
    };

    struct NativeDirectoryEnumerator final
    {
        DIR* Handle;
    };

    struct NativeMemoryMappedFileHandle final
    {
        int Descriptor;
    };
}
