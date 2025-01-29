#include <print>
#include <string_view>
#include <optional>
#include <charconv>
#include <array>
#include <span>
#include <vector>

#define NOMINMAX
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
    PVOID XmmRegisters;
    PVOID YmmRegisters;
    PVOID ZmmRegisters;
    PVOID ZmmhRegisters;
    DWORD XmmRegistersLength;
    DWORD YmmRegistersLength;
    DWORD ZmmRegistersLength;
    DWORD ZmmhRegistersLength;
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

void LogRegister64(const char* name, auto const& value) noexcept
{
    std::println("  {:<8} {:016X}", name, value);
}

void LogFpuRegister(size_t index, M128A const& value) noexcept
{
    std::println("  FPU{:<5} {:016X} {:016X}",
        index,
        static_cast<uint64_t>(value.High),
        static_cast<uint64_t>(value.Low));
}

struct M256 final
{
    M128A Low;
    M128A High;
};

struct M512 final
{
    M256 Low;
    M256 High;
};

void LogXmmRegister(size_t index, M128A const& value) noexcept
{
    std::println("  XMM{:<5} {:016X} {:016X}",
        index,
        static_cast<uint64_t>(value.High),
        static_cast<uint64_t>(value.Low));
}

void LogYmmRegister(size_t index, M256 const& value) noexcept
{
    std::println("  YMM{:<5} {:016X} {:016X} {:016X} {:016X}",
        index,
        static_cast<uint64_t>(value.High.High),
        static_cast<uint64_t>(value.High.Low),
        static_cast<uint64_t>(value.Low.High),
        static_cast<uint64_t>(value.Low.Low));
}

void LogZmmRegister(size_t index, M512 const& value) noexcept
{
    std::println("  ZMM{:<5} {:016X} {:016X} {:016X} {:016X} {:016X} {:016X} {:016X} {:016X}",
        index,
        static_cast<uint64_t>(value.High.High.High),
        static_cast<uint64_t>(value.High.High.Low),
        static_cast<uint64_t>(value.High.Low.High),
        static_cast<uint64_t>(value.High.Low.Low),
        static_cast<uint64_t>(value.Low.High.High),
        static_cast<uint64_t>(value.Low.High.Low),
        static_cast<uint64_t>(value.Low.Low.High),
        static_cast<uint64_t>(value.Low.Low.Low));
}

#if defined(_M_ARM64)
void LogNeonRegister(size_t index, ARM64_NT_NEON128 const& value) noexcept
{
    std::println("  V{:<7} {:016X} {:016X}",
        index,
        static_cast<uint64_t>(value.High),
        static_cast<uint64_t>(value.Low));
}
#endif


void PrintContext(HANDLE hProcess, CrashDetails const& context)
{
#if defined(_M_AMD64)
    if (context.Context.ContextFlags & CONTEXT_INTEGER)
    {
        std::println("RAX: {:016X} RBX: {:016X} RCX: {:016X} RDX: {:016X}",
            context.Context.Rax,
            context.Context.Rbx,
            context.Context.Rcx,
            context.Context.Rdx);

        std::println("R8:  {:016X} R9:  {:016X} R10: {:016X} R11: {:016X}",
            context.Context.R8,
            context.Context.R9,
            context.Context.R10,
            context.Context.R11);

        std::println("R12: {:016X} R13: {:016X} R14: {:016X} R15: {:016X}",
            context.Context.R12,
            context.Context.R13,
            context.Context.R14,
            context.Context.R15);

        std::println("RDI: {:016X} RSI: {:016X} RBP: {:016X} RSP: {:016X}",
            context.Context.Rdi,
            context.Context.Rsi,
            context.Context.Rbp,
            context.Context.Rsp);

        std::println("RIP: {:016X}",
            context.Context.Rip);
    }
#endif

#if defined(_M_ARM64)
    (void)hProcess;
    (void)context;

    if (context.Context.ContextFlags & CONTEXT_INTEGER)
    {
        LogRegister64("X0", context.Context.X0);
        LogRegister64("X1", context.Context.X1);
        LogRegister64("X2", context.Context.X2);
        LogRegister64("X3", context.Context.X3);
        LogRegister64("X4", context.Context.X4);
        LogRegister64("X5", context.Context.X5);
        LogRegister64("X6", context.Context.X6);
        LogRegister64("X7", context.Context.X7);
        LogRegister64("X8", context.Context.X8);
        LogRegister64("X9", context.Context.X9);
        LogRegister64("X10", context.Context.X10);
        LogRegister64("X11", context.Context.X11);
        LogRegister64("X12", context.Context.X12);
        LogRegister64("X13", context.Context.X13);
        LogRegister64("X14", context.Context.X14);
        LogRegister64("X15", context.Context.X15);
        LogRegister64("X16", context.Context.X16);
        LogRegister64("X17", context.Context.X17);
        LogRegister64("X18", context.Context.X18);
        LogRegister64("X19", context.Context.X19);
        LogRegister64("X20", context.Context.X20);
        LogRegister64("X21", context.Context.X21);
        LogRegister64("X22", context.Context.X22);
        LogRegister64("X23", context.Context.X23);
        LogRegister64("X24", context.Context.X24);
        LogRegister64("X25", context.Context.X25);
        LogRegister64("X26", context.Context.X26);
        LogRegister64("X27", context.Context.X27);
        LogRegister64("X28", context.Context.X28);
        LogRegister64("FP", context.Context.Fp);
        LogRegister64("LR", context.Context.Lr);
        LogRegister64("SP", context.Context.Sp);
        LogRegister64("PC", context.Context.Pc);
    }

    if (context.Context.ContextFlags & CONTEXT_FLOATING_POINT)
    {
        for (size_t i = 0; i < 32; ++i)
        {
            LogNeonRegister(i, context.Context.V[i]);
        }

        LogRegister64("FPCR", context.Context.Fpcr);
        LogRegister64("FPSR", context.Context.Fpsr);
    }

#endif

#if defined(_M_AMD64)
    if (context.Context.ContextFlags & CONTEXT_CONTROL)
    {
        std::println("EFL: {:08X}",
            context.Context.EFlags);
    }
#endif

#if defined(_M_AMD64)
    if (context.Context.ContextFlags & CONTEXT_SEGMENTS)
    {
        std::println("CS:  {:04X} SS:  {:04X} DS:  {:04X} ES:  {:04X} FS:  {:04X} GS:  {:04X}",
            context.Context.SegCs,
            context.Context.SegSs,
            context.Context.SegDs,
            context.Context.SegEs,
            context.Context.SegFs,
            context.Context.SegGs);
    }
#endif

#if defined(_M_AMD64)
    if (context.Context.ContextFlags & CONTEXT_DEBUG_REGISTERS)
    {
        std::println("DR0: {:016X} DR1: {:016X} DR2: {:016X} DR3: {:016X}",
            context.Context.Dr0,
            context.Context.Dr1,
            context.Context.Dr2,
            context.Context.Dr3);

        std::println("DR6: {:016X} DR7: {:016X}",
            context.Context.Dr6,
            context.Context.Dr7);
    }
#endif

#if defined(_M_AMD64)
    if (context.Context.ContextFlags & CONTEXT_XSTATE)
    {
        if (context.XmmRegisters != nullptr)
        {
            std::vector<M128A> buffer_xmm{};
            buffer_xmm.resize(context.XmmRegistersLength / sizeof(M128A));

            if (not ReadProcessMemory(hProcess, context.XmmRegisters, buffer_xmm.data(), buffer_xmm.size() * sizeof(M128A), nullptr))
            {
                std::println(stderr, "Failed to read XMM registers: {}", GetLastError());
            }

            if (context.YmmRegisters != nullptr)
            {
                std::vector<M128A> buffer_ymm{};
                buffer_ymm.resize(context.YmmRegistersLength / sizeof(M128A));

                if (not ReadProcessMemory(hProcess, context.YmmRegisters, buffer_ymm.data(), buffer_ymm.size() * sizeof(M128A), nullptr))
                {
                    std::println(stderr, "Failed to read YMM registers: {}", GetLastError());
                }

                if (context.ZmmRegisters != nullptr && context.ZmmhRegisters != nullptr)
                {
                    std::vector<M128A> buffer_zmm{};
                    buffer_zmm.resize(context.ZmmRegistersLength / sizeof(M128A));

                    if (not ReadProcessMemory(hProcess, context.ZmmRegisters, buffer_zmm.data(), buffer_zmm.size() * sizeof(M128A), nullptr))
                    {
                        std::println(stderr, "Failed to read ZMM registers: {}", GetLastError());
                    }

                    std::vector<M128A> buffer_zmmh{};
                    buffer_zmmh.resize(context.ZmmhRegistersLength / sizeof(M128A));

                    if (not ReadProcessMemory(hProcess, context.ZmmhRegisters, buffer_zmmh.data(), buffer_zmmh.size() * sizeof(M128A), nullptr))
                    {
                        std::println(stderr, "Failed to read ZMMH registers: {}", GetLastError());
                    }

                    for (size_t i = 0; i < (context.ZmmRegistersLength / sizeof(M128A)); ++i)
                    {
                        LogZmmRegister(i, M512{{buffer_xmm[i], buffer_ymm[i]}, {buffer_zmm[i], buffer_zmmh[i]}});
                    }
                }
                else
                {
                    for (size_t i = 0; i < (context.YmmRegistersLength / sizeof(M128A)); ++i)
                    {
                        LogYmmRegister(i, M256{buffer_xmm[i], buffer_ymm[i]});
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < (context.XmmRegistersLength / sizeof(M128A)); ++i)
                {
                    LogXmmRegister(i, buffer_xmm[i]);
                }
            }
        }

        for (size_t i = 0; i < 8; ++i)
        {
            LogFpuRegister(i, context.Context.FltSave.FloatRegisters[i]);
        }
    }
#endif
}

void LogBinaryHexDump(std::span<std::byte> buffer, uintptr_t base) noexcept
{
    size_t const length = buffer.size() / 16u;
    buffer = buffer.subspan(0, length * 16u);

    size_t index = 0;

    while (not buffer.empty())
    {
        std::span const line = buffer.subspan(0u, 16u);
        buffer = buffer.subspan(16u);

        std::print("  #{:02} {:016x}", index, base + (index * 16));

        std::print(" |");

        for (std::byte const byte : line)
        {
            std::print(" {:02x}", static_cast<uint8_t>(byte));
        }

        std::print(" |");

        for (std::byte const byte : line)
        {
            int ch = static_cast<std::uint8_t>(byte);

            if (isgraph(ch))
            {
                std::print(" {}", static_cast<char>(ch));
            }
            else
            {
                std::print(" .");
            }
        }

        std::println();
        ++index;
    }
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

    std::fwprintf(stderr, L"Opened process\n");
    // Read crash details from process.
    auto crashDetails = ReadCrashDetails(*options.ProcessId, *options.ThreadId);

    if (crashDetails == std::nullopt)
    {
        std::println(stderr, "Failed to read crash details: {}", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    std::fwprintf(stderr, L"Got details\n");

    PrintContext(hProcess, *crashDetails);

    {
        std::array<std::byte, 128> buffer{};

        size_t processed = 0;
#if defined(_M_AMD64)
        if (ReadProcessMemory(hProcess, std::bit_cast<LPCVOID>(crashDetails->Context.Rip), buffer.data(), buffer.size(), &processed))
        {
            LogBinaryHexDump(buffer, crashDetails->Context.Rip);
        }
#elif defined(_M_ARM64)
        if (ReadProcessMemory(hProcess, std::bit_cast<LPCVOID>(crashDetails->Context.Sp), buffer.data(), buffer.size(), &processed))
        {
            LogBinaryHexDump(buffer, crashDetails->Context.Sp);
        }
#endif

        processed = 0;

#if defined(_M_AMD64)
        if (ReadProcessMemory(hProcess, std::bit_cast<LPCVOID>(crashDetails->Context.Rsp), buffer.data(), buffer.size(), &processed))
        {
            LogBinaryHexDump(buffer, crashDetails->Context.Rip);
        }
#elif defined(_M_ARM64)
        if (ReadProcessMemory(hProcess, std::bit_cast<LPCVOID>(crashDetails->Context.Pc), buffer.data(), buffer.size(), &processed))
        {
            LogBinaryHexDump(buffer, crashDetails->Context.Pc);
        }
#endif
    }

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
