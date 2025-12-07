#pragma once
#include "AnemoneRuntime.Interop/Windows/Headers.hxx"

#include <cstdio>

#if defined(ANEMONE_EXECUTABLE_SINGLE_INSTANCE_ID)

HANDLE gSingleInstanceMutex{};

void AnemoneAcquireSingleInstanceMutex()
{
    bool first = true;

    const wchar_t* mutexName = L"Global\\AnemoneSingleInstanceMutex_" ANEMONE_EXECUTABLE_SINGLE_INSTANCE_ID;

    gSingleInstanceMutex = CreateMutexW(nullptr, TRUE, mutexName);

    if (gSingleInstanceMutex != nullptr)
    {
        DWORD const dwError = GetLastError();

        if (dwError == ERROR_ALREADY_EXISTS)
        {
            first = false;

            ReleaseMutex(gSingleInstanceMutex);
            gSingleInstanceMutex = nullptr;
        }
    }
}

void AnemoneReleaseSingleInstanceMutex()
{
    if (gSingleInstanceMutex != nullptr)
    {
        ReleaseMutex(gSingleInstanceMutex);
        CloseHandle(gSingleInstanceMutex);
        gSingleInstanceMutex = nullptr;
    }
}

#endif
