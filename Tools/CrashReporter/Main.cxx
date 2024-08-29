#include <print>
#include <string_view>
#include <optional>
#include <charconv>

#define WIN32_LEAN_AND_MEAN
#include <initguid.h>
#include <Windows.h>
#include <DbgHelp.h>

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

int main(int argc, char* argv[])
{
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

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, *options.ProcessId);
    if (hProcess == nullptr)
    {
        std::println(stderr, "Failed to open process: {}", GetLastError());
        return 1;
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

    if (!MiniDumpWriteDump(hProcess, *options.ProcessId, hFile, static_cast<MINIDUMP_TYPE>(dumpType), nullptr, nullptr, nullptr))
    {
        std::println(stderr, "Failed to write dump: {}", GetLastError());
        CloseHandle(hFile);
        CloseHandle(hProcess);
        return 1;
    }

    CloseHandle(hFile);
    CloseHandle(hProcess);

    std::wprintf(L"Dump written to %s\n", wFileName);

    return 0;
}
