#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

namespace Anemone::Interop::Windows
{
    anemone_forceinline bool IsProcessForeground()
    {
        DWORD dwProcessId{};
        GetWindowThreadProcessId(GetForegroundWindow(), &dwProcessId);
        return dwProcessId == GetCurrentProcessId();
    }

    inline bool IsProcessEmulated()
    {
        USHORT processMachine = IMAGE_FILE_MACHINE_UNKNOWN;
        USHORT nativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
    
        if (IsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine))
        {
    #if ANEMONE_ARCHITECTURE_X64
            if (nativeMachine != IMAGE_FILE_MACHINE_AMD64)
            {
                return true;
            }
    #endif
    
    #if ANEMONE_ARCHITECTURE_ARM64
            if (nativeMachine != IMAGE_FILE_MACHINE_ARM64)
            {
                return true;
            }
    #endif
        }
    
        return false;
    }
    
    template <size_t CapacityT>
    anemone_forceinline bool QueryFullProcessImageName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        HANDLE const hProcess = GetCurrentProcess();
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);

            if (QueryFullProcessImageNameW(hProcess, 0, buffer.data(), &dwSize))
            {
                capacity = dwSize;
                return true;
            }

            if (DWORD const dwError = GetLastError(); dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                capacity = static_cast<size_t>(dwSize) * 2uz;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetModuleFileName(string_buffer<wchar_t, CapacityT>& result, HMODULE handle)
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  The size of the lpFilename buffer, in WCHARs.
            SetLastError(ERROR_SUCCESS);
            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);

            DWORD dwLength = GetModuleFileNameW(handle, buffer.data(), dwSize);

            if (dwLength == 0)
            {
                // MSDN:
                //  If the function fails, the return value is 0 (zero).
                capacity = 0;
                return false;
            }

            if (DWORD const dwError = GetLastError(); dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                // MSDN:
                //  If the buffer is too small to hold the module name, the string is truncated to nSize characters
                //  including the terminating null character, the function returns nSize, and the function sets the
                //  last error to ERROR_INSUFFICIENT_BUFFER.

                // Try to double capacity if the buffer is too small.
                dwLength *= 2u;
            }

            capacity = dwLength;
            return true;
        });
    }

    anemone_forceinline bool GetThreadContext(DWORD threadId, CONTEXT& context)
    {
        context = {};
        context.ContextFlags = CONTEXT_ALL;

        bool result = false;

        if (threadId == GetCurrentThreadId())
        {
            RtlCaptureContext(&context);
            result = true;
        }
        else
        {
            HANDLE const hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, threadId);

            if (hThread != nullptr)
            {
                DWORD const dwSuspended = SuspendThread(hThread);

                if (dwSuspended != static_cast<DWORD>(-1))
                {
                    ::GetThreadContext(hThread, &context);
                    ResumeThread(hThread);
                    result = true;
                }

                CloseHandle(hThread);
            }
        }

        return result;
    }

    anemone_forceinline PIMAGE_NT_HEADERS GetImageNtHeaders(HMODULE h)
    {
        return std::bit_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<UINT_PTR>(h) + reinterpret_cast<PIMAGE_DOS_HEADER>(h)->e_lfanew);
    }

    anemone_forceinline bool IsConsoleApplication(HMODULE h)
    {
        PIMAGE_NT_HEADERS const ntHeaders = GetImageNtHeaders(h);
        return (ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);
    }

    anemone_forceinline bool IsConsoleHandle(HANDLE h)
    {
        DWORD dwMode = 0;
        return GetConsoleMode(h, &dwMode) != FALSE;
    }

    anemone_forceinline bool IsConsoleRedirecting(HANDLE handle)
    {
        DWORD dwType = GetFileType(handle);
        return (dwType == FILE_TYPE_CHAR) or (dwType == FILE_TYPE_PIPE);
    }
}
