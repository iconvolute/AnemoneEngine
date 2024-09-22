#include <print>
#include <string_view>
#include <optional>
#include <charconv>

#define WIN32_LEAN_AND_MEAN
#include <initguid.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <TlHelp32.h>

struct CommandLineEnumerator
{
private:
    int m_index;
    int m_argc;
    char** m_argv;

public:
    CommandLineEnumerator(int argc, char** argv)
        : m_index{1}
        , m_argc{argc}
        , m_argv{argv}
    {
    }

    bool HasNext() const
    {
        return m_index < m_argc;
    }

    std::string_view Next()
    {
        return m_argv[m_index++];
    }
};

struct ParsedOptions
{
    HRESULT PrintUsage()
    {
        std::println(stderr, "Usage: CrashReporter --pid <process-id> --file <file-name> --type <type>");
        return E_INVALIDARG;
    }

    HRESULT Parse(CommandLineEnumerator& enumerator)
    {
        while (enumerator.HasNext())
        {
            std::string_view name = enumerator.Next();

            if (name == "--pid")
            {
                if (!enumerator.HasNext())
                {
                    std::println(stderr, "--pid requires a process id.");
                    return E_INVALIDARG;
                }

                if (this->ProcessId)
                {
                    std::println(stderr, "--pid can only be specified once.");
                    return E_INVALIDARG;
                }

                std::string_view const argPid = enumerator.Next();

                DWORD dwPid{};

                if (auto [p, ec] = std::from_chars(argPid.data(), argPid.data() + argPid.size(), dwPid); ec != std::errc())
                {
                    std::println(stderr, "Invalid process id: {}", argPid);
                    return E_INVALIDARG;
                }

                if (dwPid == 0)
                {
                    std::println(stderr, "Invalid process id: 0");
                    return E_INVALIDARG;
                }

                this->ProcessId = dwPid;
            }
            else if (name == "--tid")
            {
                if (!enumerator.HasNext())
                {
                    std::println(stderr, "--file requires a file name.");
                    return E_INVALIDARG;
                }

                if (this->ThreadId)
                {
                    std::println(stderr, "--file can only be specified once.");
                    return E_INVALIDARG;
                }

                std::string_view const argTid = enumerator.Next();

                DWORD dwTid{};

                if (auto [p, ec] = std::from_chars(argTid.data(), argTid.data() + argTid.size(), dwTid); ec != std::errc())
                {
                    std::println(stderr, "Invalid thread id: {}", argTid);
                    return E_INVALIDARG;
                }

                if (dwTid == 0)
                {
                    std::println(stderr, "Invalid thread id: 0");
                    return E_INVALIDARG;
                }

                this->ThreadId = dwTid;
            }
            else if (name == "--type")
            {
                if (!enumerator.HasNext())
                {
                    std::println(stderr, "--type requires a type.");
                    return E_INVALIDARG;
                }

                if (this->FullDump)
                {
                    std::println(stderr, "--type can only be specified once.");
                    return E_INVALIDARG;
                }

                std::string_view const argType = enumerator.Next();

                if (argType == "full")
                {
                    this->FullDump = true;
                }
                else if (argType == "mini")
                {
                    this->FullDump = false;
                }
                else
                {
                    std::println(stderr, "Invalid type: {}", argType);
                    return E_INVALIDARG;
                }
            }
            else
            {
                std::println(stderr, "Invalid option: {}", name);
                return PrintUsage();
            }
        }

        return S_OK;
    }

    std::optional<DWORD> ProcessId{};
    std::optional<DWORD> ThreadId{};
    std::optional<bool> FullDump{};
};

struct CrashDetails
{
    DWORD ProcessId;
    DWORD ThreadId;
    EXCEPTION_RECORD ExceptionRecord;
    CONTEXT Context;
};

HRESULT SuspendAllThreadsExcept(DWORD pid, DWORD tid)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }

    THREADENTRY32 te{};
    te.dwSize = sizeof(te);

    if (!Thread32First(hSnapshot, &te))
    {
        CloseHandle(hSnapshot);
        return E_FAIL;
    }

    do
    {
        if (te.th32OwnerProcessID == pid && te.th32ThreadID != tid)
        {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
            if (hThread != nullptr)
            {
                SuspendThread(hThread);
                CloseHandle(hThread);
            }
        }
    } while (Thread32Next(hSnapshot, &te));

    CloseHandle(hSnapshot);

    return S_OK;
}

std::optional<CrashDetails> ReadCrashDetails(DWORD processId, DWORD threadId)
{
    wchar_t name[MAX_PATH];
    swprintf_s(name, L"AnemoneCrashDetails_pid%u_tid%u", static_cast<UINT>(processId), static_cast<UINT>(threadId));

    HANDLE hMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name);
    if (hMapping == nullptr)
    {
        return std::nullopt;
    }

    void* buffer = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CrashDetails));
    if (buffer == nullptr)
    {
        CloseHandle(hMapping);
        return std::nullopt;
    }

    CrashDetails crashDetails{};

    CopyMemory(&crashDetails, buffer, sizeof(CrashDetails));

    UnmapViewOfFile(buffer);

    CloseHandle(hMapping);

    if (crashDetails.ProcessId != processId || crashDetails.ThreadId != threadId)
    {
        std::println(stderr, "Invalid crash details\n");
        abort();
    }

    return crashDetails;
}

void EnableDebugPrivileges()
{
    HANDLE hToken = nullptr;
    TOKEN_PRIVILEGES tkp{};

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        wprintf(L"Failed to open process token.\n");
        return;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    LPCWSTR lpwPriv = L"SeDebugPrivilege";
    if (!LookupPrivilegeValueW(NULL, lpwPriv, &tkp.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return;
    }

    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
    {
        wprintf(L"Failed to adjust process token.\n");
    }

    CloseHandle(hToken);
}

int main(int argc, char* argv[])
{
    EnableDebugPrivileges();
    CommandLineEnumerator enumerator{argc, argv};

    ParsedOptions options{};

    if (FAILED(options.Parse(enumerator)))
    {
        return 1;
    }

    if (!options.ProcessId)
    {
        std::println(stderr, "--pid is required.");
        return 1;
    }

    if (!options.ThreadId)
    {
        std::println(stderr, "--tid is required.");
        return 1;
    }

    if (FAILED(SuspendAllThreadsExcept(*options.ProcessId, *options.ThreadId)))
    {
        std::println(stderr, "Failed to suspend threads: {}", GetLastError());
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, *options.ProcessId);
    if (hProcess == nullptr)
    {
        std::println(stderr, "Failed to open process: {}", GetLastError());
        return 1;
    }

    std::fwprintf(stderr, L"Opened process");
    // Read crash details from process.
    auto crashDetails = ReadCrashDetails(*options.ProcessId, *options.ThreadId);

    if (crashDetails == std::nullopt)
    {
        std::println(stderr, "Failed to read crash details: {}", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    std::fwprintf(stderr, L"Got details\n");

    SYSTEMTIME st{};
    GetLocalTime(&st);

    wchar_t wFileName[128];

    if (swprintf_s(
            wFileName,
            L"pid-%u-tid-%u-%04u-%02u-%02u-%02u-%02u-%02u.dmp",
            static_cast<UINT>(*options.ProcessId),
            static_cast<UINT>(*options.ThreadId),
            st.wYear,
            st.wMonth,
            st.wDay,
            st.wHour,
            st.wMinute,
            st.wSecond) < 0)
    {
        std::println(stderr, "Failed to format dump name: {}", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hFile = CreateFileW(
        wFileName,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::println(stderr, "Failed to create file: {}", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    auto dumpType = options.FullDump.value_or(false)
        ? (MiniDumpWithFullMemory | MiniDumpWithUnloadedModules | MiniDumpWithAvxXStateContext)
        : (MiniDumpNormal);

    EXCEPTION_POINTERS exceptionPointers;
    exceptionPointers.ExceptionRecord = &crashDetails->ExceptionRecord;
    exceptionPointers.ContextRecord = &crashDetails->Context;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = *options.ThreadId;
    exceptionInfo.ExceptionPointers = &exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;

    std::fwprintf(stderr, L"Writing dump to %s\n", wFileName);

    for (size_t i = 0; i < 10; ++i)
    {
        if (!MiniDumpWriteDump(
                hProcess,
                *options.ProcessId,
                hFile,
                static_cast<MINIDUMP_TYPE>(dumpType),
                &exceptionInfo,
                nullptr,
                nullptr))
        {
            if (static_cast<HRESULT>(GetLastError()) != HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY))
            {
                std::fwprintf(stderr, L"Failed to write dump: %lx\n", GetLastError());
                CloseHandle(hFile);
                CloseHandle(hProcess);
                return 1;
            }
            else
            {
                std::fwprintf(stderr, L"Partial copy error, retrying...\n");
                Sleep(1000);
            }
        }
        else
        {
            break;
        }
    }

    CloseHandle(hFile);
    CloseHandle(hProcess);

    std::wprintf(L"Dump written to %s\n", wFileName);

    return 0;
}
