#include <print>
#include <string_view>
#include <optional>
#include <charconv>
#include <array>
#include <span>
#include <vector>
#include <source_location>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <initguid.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <TlHelp32.h>

#include <wrl/client.h>
#include <DbgEng.h>

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
    std::optional<bool> FullDump{};
};

class MyOutput : public IDebugOutputCallbacks
{
public:
    MyOutput(Microsoft::WRL::ComPtr<IDebugClient9> p_client)
        : p_client_{p_client}
    {
    }

    virtual ~MyOutput() = default;
    MyOutput(const MyOutput&) = delete;
    MyOutput& operator=(const MyOutput&) = delete;

    // Inherited via IDebugOutputCallbacks
    HRESULT __stdcall QueryInterface(REFIID InterfaceId, PVOID* Interface) override
    {
        (void)InterfaceId;
        (void)Interface;
        return E_NOTIMPL;
    }

    ULONG __stdcall AddRef() override
    {
        return 0;
    }

    ULONG __stdcall Release() override
    {
        return 0;
    }

    HRESULT __stdcall Output(ULONG Mask, PCSTR Text) override
    {
        if (Mask & DEBUG_OUTPUT_NORMAL)
        {
            std::print("{}", Text);
        }

        return S_OK;
    }

private:
    Microsoft::WRL::ComPtr<IDebugClient9> p_client_;
};

class MyCallback : public IDebugEventCallbacks
{
public:
    MyCallback() = default;
    virtual ~MyCallback() = default;
    // Inherited via IDebugEventCallbacks
    HRESULT __stdcall QueryInterface(REFIID InterfaceId,
        PVOID* Interface) override;
    ULONG __stdcall AddRef(void) override;
    ULONG __stdcall Release(void) override;
    HRESULT __stdcall GetInterestMask(PULONG Mask) override;
    HRESULT __stdcall Breakpoint(PDEBUG_BREAKPOINT Bp) override;
    HRESULT __stdcall Exception(PEXCEPTION_RECORD64 Exception,
        ULONG FirstChance) override;
    HRESULT __stdcall CreateThread(ULONG64 Handle, ULONG64 DataOffset,
        ULONG64 StartOffset) override;
    HRESULT __stdcall ExitThread(ULONG ExitCode) override;
    HRESULT __stdcall ExitProcess(ULONG ExitCode) override;
    HRESULT __stdcall LoadModule(ULONG64 ImageFileHandle, ULONG64 BaseOffset,
        ULONG ModuleSize, PCSTR ModuleName,
        PCSTR ImageName, ULONG CheckSum,
        ULONG TimeDateStamp) override;
    HRESULT __stdcall UnloadModule(PCSTR ImageBaseName,
        ULONG64 BaseOffset) override;
    HRESULT __stdcall SystemError(ULONG Error, ULONG Level) override;
    HRESULT __stdcall SessionStatus(ULONG Status) override;
    HRESULT __stdcall ChangeDebuggeeState(ULONG Flags, ULONG64 Argument) override;
    HRESULT __stdcall ChangeEngineState(ULONG Flags, ULONG64 Argument) override;
    HRESULT __stdcall ChangeSymbolState(ULONG Flags, ULONG64 Argument) override;
    HRESULT __stdcall CreateProcessW(ULONG64 ImageFileHandle, ULONG64 Handle,
        ULONG64 BaseOffset, ULONG ModuleSize,
        PCSTR ModuleName, PCSTR ImageName,
        ULONG CheckSum, ULONG TimeDateStamp,
        ULONG64 InitialThreadHandle,
        ULONG64 ThreadDataOffset,
        ULONG64 StartOffset) override;
};

HRESULT __stdcall MyCallback::QueryInterface(REFIID InterfaceId,
    PVOID* Interface)
{
    (void)InterfaceId;
    (void)Interface;
    return E_NOTIMPL;
}
ULONG __stdcall MyCallback::AddRef() { return 0; }
ULONG __stdcall MyCallback::Release() { return 0; }
HRESULT __stdcall MyCallback::GetInterestMask(PULONG Mask)
{
    *Mask = 0;
    return S_OK;
}
HRESULT __stdcall MyCallback::Breakpoint(PDEBUG_BREAKPOINT Bp)
{
    ULONG64 bp_offset;
    HRESULT hr = Bp->GetOffset(&bp_offset);
    if (FAILED(hr))
        return hr;
    // Break on breakpoints? Seems reasonable.
    return DEBUG_STATUS_BREAK;
}
HRESULT __stdcall MyCallback::Exception(PEXCEPTION_RECORD64 Exception,
    ULONG FirstChance)
{
    std::println("Exception: {:016X} FirstChance: {}",
        Exception->ExceptionAddress,
        FirstChance);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::CreateThread(ULONG64 Handle, ULONG64 DataOffset,
    ULONG64 StartOffset)
{
    std::println("CreateThread: {:016X} DataOffset: {:016X} StartOffset: {:016X}",
        Handle,
        DataOffset,
        StartOffset);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::ExitThread(ULONG ExitCode)
{
    std::println("ExitThread: {}", ExitCode);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::ExitProcess(ULONG ExitCode)
{
    std::println("ExitProcess: {}", ExitCode);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::LoadModule(ULONG64 ImageFileHandle,
    ULONG64 BaseOffset, ULONG ModuleSize,
    PCSTR ModuleName, PCSTR ImageName,
    ULONG CheckSum, ULONG TimeDateStamp)
{
    std::println("LoadModule: {:016X} BaseOffset: {:016X} ModuleSize: {} ModuleName: {} ImageName: {} CheckSum: {} TimeDateStamp: {}",
        ImageFileHandle,
        BaseOffset,
        ModuleSize,
        ModuleName,
        ImageName,
        CheckSum,
        TimeDateStamp);

    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::UnloadModule(PCSTR ImageBaseName,
    ULONG64 BaseOffset)
{
    std::println("UnloadModule: {} BaseOffset: {:016X}",
        ImageBaseName,
        BaseOffset);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::SystemError(ULONG Error, ULONG Level)
{
    std::println("SystemError: {} Level: {}", Error, Level);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::SessionStatus(ULONG Status)
{
    std::println("SessionStatus: {}", Status);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::ChangeDebuggeeState(ULONG Flags,
    ULONG64 Argument)
{
    std::println("ChangeDebuggeeState: {} Argument: {:016X}",
        Flags,
        Argument);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::ChangeEngineState(ULONG Flags, ULONG64 Argument)
{
    std::println("ChangeEngineState: {} Argument: {:016X}",
        Flags,
        Argument);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::ChangeSymbolState(ULONG Flags, ULONG64 Argument)
{
    std::println("ChangeSymbolState: {} Argument: {:016X}",
        Flags,
        Argument);
    return E_NOTIMPL;
}
HRESULT __stdcall MyCallback::CreateProcessW(
    ULONG64 ImageFileHandle, ULONG64 Handle, ULONG64 BaseOffset,
    ULONG ModuleSize, PCSTR ModuleName, PCSTR ImageName, ULONG CheckSum,
    ULONG TimeDateStamp, ULONG64 InitialThreadHandle, ULONG64 ThreadDataOffset,
    ULONG64 StartOffset)
{
    std::println("CreateProcessW: {:016X} Handle: {:016X} BaseOffset: {:016X} ModuleSize: {} ModuleName: {} ImageName: {} CheckSum: {} TimeDateStamp: {} InitialThreadHandle: {:016X} ThreadDataOffset: {:016X} StartOffset: {:016X}",
        ImageFileHandle,
        Handle,
        BaseOffset,
        ModuleSize,
        ModuleName,
        ImageName,
        CheckSum,
        TimeDateStamp,
        InitialThreadHandle,
        ThreadDataOffset,
        StartOffset);

    // Should fire once the target process is launched. Break to create
    // breakpoints, etc.
    return DEBUG_STATUS_BREAK;
}

void AbortIfFailed(HRESULT hr, std::source_location const& location = std::source_location::current())
{
    if (FAILED(hr))
    {
        std::println(stderr, "{}:{}: Failed {:08x}", location.file_name(), location.line(), static_cast<uint32_t>(hr));
        std::abort();
    }
}

int main(int argc, char* argv[])
{
    CommandLineEnumerator enumerator{argc, argv};

    ParsedOptions options{};

    AbortIfFailed(options.Parse(enumerator));

    if (!options.ProcessId)
    {
        std::println(stderr, "--pid is required.");
        return 1;
    }

    DWORD pid = options.ProcessId.value();

    if (argc)
    {
        Microsoft::WRL::ComPtr<IDebugClient9> debugClient{};
        MyOutput output{debugClient};
        MyCallback callback{};

        std::println("DebuggerPid: {}, ClientPid: {}",
            GetCurrentProcessId(),
            options.ProcessId.value());

        AbortIfFailed(DebugCreate(IID_PPV_ARGS(debugClient.GetAddressOf())));
        
        Microsoft::WRL::ComPtr<IDebugAdvanced4> debugAdvanced{};
        AbortIfFailed(debugClient->QueryInterface(IID_PPV_ARGS(debugAdvanced.GetAddressOf())));
        
        Microsoft::WRL::ComPtr<IDebugControl> debugControl{};
        AbortIfFailed(debugClient->QueryInterface(IID_PPV_ARGS(debugControl.GetAddressOf())));
        AbortIfFailed(debugControl->AddEngineOptions(DEBUG_ENGOPT_INITIAL_BREAK));

        AbortIfFailed(debugClient->SetOutputCallbacks(&output));
        AbortIfFailed(debugClient->SetEventCallbacks(&callback));

        AbortIfFailed(debugClient->AttachProcess(0, pid, DEBUG_ATTACH_NONINVASIVE));
        AbortIfFailed(debugControl->WaitForEvent(0, INFINITE));

        ULONG executeMode = DEBUG_EXECUTE_DEFAULT;

        // Set frames limit 128 entries
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, ".kframes 0n128", executeMode));

        // List modules
        std::println("=======");
        std::println("Modules");
        std::println("=======");
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "lm", executeMode));
        std::println();

        // Print "parallel stacks"

        std::println("===============");
        std::println("Parallel Stacks");
        std::println("===============");
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "~*k", executeMode));
        std::println();

        // Print registers
        std::println("=========");
        std::println("Registers");
        std::println("=========");
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "r", executeMode));
        std::println();

        // Print exception record
        std::println("================");
        std::println("Exception Record");
        std::println("================");
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, ".exr -1", executeMode));
        std::println();

        // Disassemble code at place where crashed
        std::println("===========");
        std::println("Disassembly");
        std::println("===========");
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "ub @$ip", executeMode));
        AbortIfFailed(debugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "u @$ip", executeMode));
        std::println();

        ULONG flags{};
        flags |= DEBUG_FORMAT_USER_SMALL_FULL_MEMORY;
        flags |= DEBUG_FORMAT_USER_SMALL_HANDLE_DATA;
        flags |= DEBUG_FORMAT_USER_SMALL_PROCESS_THREAD_DATA;
        flags |= DEBUG_FORMAT_USER_SMALL_THREAD_INFO;
        flags |= DEBUG_FORMAT_USER_SMALL_CODE_SEGMENTS;
        flags |= DEBUG_FORMAT_USER_SMALL_FULL_MEMORY_INFO;
        flags |= DEBUG_FORMAT_USER_SMALL_DATA_SEGMENTS;

        AbortIfFailed(debugClient->WriteDumpFile2("e:\\test.dmp", DEBUG_DUMP_SMALL, flags, "Comment!"));

        AbortIfFailed(debugClient->DetachProcesses());

        return 2137;
    }

    return 0;
}
