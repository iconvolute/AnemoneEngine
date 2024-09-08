#pragma once
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Platform
{
    struct CrashDetails
    {
        DWORD ProcessId;
        DWORD ThreadId;
        EXCEPTION_RECORD ExceptionRecord;
        CONTEXT Context;
    };

    struct Mapping
    {
        HANDLE Handle;
        void* Buffer;

        Mapping(DWORD processId, DWORD threadId)
        {
            wchar_t name[MAX_PATH];
            swprintf_s(name, L"AnemoneCrashDetails_pid%u_tid%u", static_cast<UINT>(processId), static_cast<UINT>(threadId));

            Handle = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                nullptr,
                PAGE_READWRITE,
                0,
                sizeof(CrashDetails),
                name);
            if (Handle == nullptr)
            {
                return;
            }

            Buffer = MapViewOfFile(Handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CrashDetails));
        }

        ~Mapping()
        {
            if (Buffer != nullptr)
            {
                UnmapViewOfFile(Buffer);
            }

            if (Handle != nullptr)
            {
                CloseHandle(Handle);
            }
        }

        void Write(CrashDetails const& crashDetails)
        {
            if (Buffer != nullptr)
            {
                CopyMemory(Buffer, &crashDetails, sizeof(CrashDetails));
            }

            FlushViewOfFile(Buffer, sizeof(CrashDetails));
        }
    };
}
