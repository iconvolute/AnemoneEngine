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

#include <comip.h>
#include <wrl/client.h>
#include <DbgEng.h>

Microsoft::WRL::ComPtr<IDebugClient9> GDebugClient{};
Microsoft::WRL::ComPtr<IDebugAdvanced4> GDebugAdvanced{};
Microsoft::WRL::ComPtr<IDebugControl> GDebugControl{};
Microsoft::WRL::ComPtr<IDebugRegisters2> GDebugRegisters{};
Microsoft::WRL::ComPtr<IDebugSymbols5> GDebugSymbols{};

std::string ReportErrorWin32(DWORD error)
{
    CHAR szMessageBuffer[512];

    DWORD dwChars = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        szMessageBuffer,
        512,
        nullptr);

    if (0 == dwChars)
    {
        HINSTANCE hInst = LoadLibraryW(L"Ntdsbmsg.dll");

        if (hInst)
        {
            dwChars = FormatMessageA(
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                hInst,
                error,
                0,
                szMessageBuffer,
                512,
                nullptr);

            FreeLibrary(hInst);
        }
    }

    if (dwChars > 3)
    {
        if (szMessageBuffer[dwChars - 1] == L'\n')
        {
            --dwChars;
        }

        if (szMessageBuffer[dwChars - 1] == L'\r')
        {
            --dwChars;
        }

        if (szMessageBuffer[dwChars - 1] == L'.')
        {
            --dwChars;
        }

        szMessageBuffer[dwChars] = L'\0';
    }

    return szMessageBuffer;
}

std::string ReportErrorHRESULT(HRESULT hresult)
{
    return ReportErrorWin32(static_cast<DWORD>(hresult));
}

void AbortIfFailed(HRESULT hr, std::source_location const& location = std::source_location::current())
{
    if (FAILED(hr))
    {
        std::println(
            stderr,
            "{}:{}: Failed {:08x} ('{}')",
            location.file_name(),
            location.line(),
            static_cast<uint32_t>(hr),
            ReportErrorHRESULT(hr));
    }
}


std::optional<std::string> GetModuleNameByAddress(DWORD64 address)
{
    CHAR moduleName[MAX_PATH];

    ULONG index{};
    ULONG64 base{};

    if (SUCCEEDED(GDebugSymbols->GetModuleByOffset(address, 0, &index, &base)))
    {
        ULONG displacement{};

        if (SUCCEEDED(GDebugSymbols->GetModuleNameString(
                DEBUG_MODNAME_MODULE,
                index,
                base,
                moduleName,
                sizeof(moduleName),
                &displacement)))
        {
            return std::string{moduleName};
        }
    }

    return std::nullopt;
}

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

    STDMETHOD(QueryInterface)(
        REFIID InterfaceId,
        PVOID* Interface) override;

    STDMETHOD_(ULONG, AddRef)() override;

    STDMETHOD_(ULONG, Release)() override;

    STDMETHOD(Output)(
        ULONG Mask,
        PCSTR Text) override;

private:
    Microsoft::WRL::ComPtr<IDebugClient9> p_client_;
};


COM_DECLSPEC_NOTHROW STDMETHODIMP MyOutput::QueryInterface(
    REFIID InterfaceId,
    PVOID* Interface)
{
    (void)InterfaceId;
    (void)Interface;
    return E_NOINTERFACE;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) MyOutput::AddRef()
{
    return 0;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) MyOutput::Release()
{
    return 0;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyOutput::Output(
    ULONG Mask,
    PCSTR Text)
{
    if (Mask & DEBUG_OUTPUT_NORMAL)
    {
        std::print("{}", Text);
    }

    return S_OK;
}

class MyCallback : public IDebugEventCallbacks
{
public:
    MyCallback() = default;
    virtual ~MyCallback() = default;

    STDMETHOD(QueryInterface)(
        REFIID InterfaceId,
        PVOID* Interface) override;

    STDMETHOD_(ULONG, AddRef)() override;

    STDMETHOD_(ULONG, Release)() override;

    STDMETHOD(GetInterestMask)(
        PULONG Mask) override;

    STDMETHOD(Breakpoint)(
        PDEBUG_BREAKPOINT Bp) override;

    STDMETHOD(Exception)(
        PEXCEPTION_RECORD64 Exception,
        ULONG FirstChance) override;

    STDMETHOD(CreateThread)(
        ULONG64 Handle,
        ULONG64 DataOffset,
        ULONG64 StartOffset) override;

    STDMETHOD(ExitThread)(
        ULONG ExitCode) override;

    STDMETHOD(ExitProcess)(
        ULONG ExitCode) override;

    STDMETHOD(LoadModule)(
        ULONG64 ImageFileHandle,
        ULONG64 BaseOffset,
        ULONG ModuleSize,
        PCSTR ModuleName,
        PCSTR ImageName,
        ULONG CheckSum,
        ULONG TimeDateStamp) override;

    STDMETHOD(UnloadModule)(
        PCSTR ImageBaseName,
        ULONG64 BaseOffset) override;

    STDMETHOD(SystemError)(
        ULONG Error,
        ULONG Level) override;

    STDMETHOD(SessionStatus)(
        ULONG Status) override;

    STDMETHOD(ChangeDebuggeeState)(
        ULONG Flags,
        ULONG64 Argument) override;

    STDMETHOD(ChangeEngineState)(
        ULONG Flags,
        ULONG64 Argument) override;

    STDMETHOD(ChangeSymbolState)(
        ULONG Flags,
        ULONG64 Argument) override;

    STDMETHOD(CreateProcessW)(
        ULONG64 ImageFileHandle,
        ULONG64 Handle,
        ULONG64 BaseOffset,
        ULONG ModuleSize,
        PCSTR ModuleName,
        PCSTR ImageName,
        ULONG CheckSum,
        ULONG TimeDateStamp,
        ULONG64 InitialThreadHandle,
        ULONG64 ThreadDataOffset,
        ULONG64 StartOffset) override;
};

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::QueryInterface(
    REFIID InterfaceId,
    PVOID* Interface)
{
    (void)InterfaceId;
    (void)Interface;
    return E_NOINTERFACE;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) MyCallback::AddRef()
{
    return 0;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP_(ULONG) MyCallback::Release()
{
    return 0;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::GetInterestMask(
    PULONG Mask)
{
    *Mask = 0;
    *Mask = DEBUG_EVENT_EXCEPTION;
    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::Breakpoint(
    PDEBUG_BREAKPOINT Bp)
{
    ULONG breakpointId{};
    HRESULT hr = Bp->GetId(&breakpointId);

    if (SUCCEEDED(hr))
    {
        std::println("Breakpoint: {:016X}", breakpointId);
    }
    else
    {
        std::println("Breakpoint: Failed to get ID");
    }

    return DEBUG_STATUS_GO;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::Exception(
    PEXCEPTION_RECORD64 Exception,
    ULONG FirstChance)
{
    std::println("Exception: {:016X} FirstChance: {}, Module: {}",
        Exception->ExceptionAddress,
        FirstChance,
        GetModuleNameByAddress(Exception->ExceptionAddress).value_or("<unknown>"));

    // std::println("--------------------");
    // AbortIfFailed(GDebugRegisters->OutputRegisters(DEBUG_OUTCTL_THIS_CLIENT, DEBUG_REGISTERS_ALL));
    // std::println("--------------------");
    // AbortIfFailed(GDebugControl->OutputCurrentState(DEBUG_OUTCTL_THIS_CLIENT, DEBUG_CURRENT_DEFAULT));
    // std::println("--------------------");
    // AbortIfFailed(GDebugControl->OutputStackTrace(DEBUG_OUTCTL_THIS_CLIENT, nullptr, 128, DEBUG_STACK_FRAME_NUMBERS | DEBUG_STACK_FRAME_ADDRESSES | DEBUG_STACK_FUNCTION_INFO));
    // std::println("--------------------");

    ULONG executeMode = DEBUG_EXECUTE_DEFAULT;
    // List modules
    std::println("=======");
    std::println("Modules");
    std::println("=======");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "lm", executeMode));
    std::println();

    // Print "parallel stacks"

    std::println("===============");
    std::println("Parallel Stacks");
    std::println("===============");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "~*k", executeMode));
    std::println();

    // Print registers
    std::println("=========");
    std::println("Registers");
    std::println("=========");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "r", executeMode));
    std::println();

    // Print exception record
    std::println("================");
    std::println("Exception Record");
    std::println("================");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, ".exr -1", executeMode));
    std::println();

    // Disassemble code at place where crashed
    std::println("===========");
    std::println("Disassembly");
    std::println("===========");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "ub @$ip", executeMode));
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "u @$ip", executeMode));
    std::println();

    // Disassemble code at place where crashed
    std::println("==============");
    std::println("Stack Contents");
    std::println("==============");
    // display pointers+symbols, 1 column, at current stack pointer
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "dps /c1 @$csp L-16", executeMode));
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "dps /c1 @$csp L16", executeMode));
    std::println();

#if false
    ULONG flags{};
    flags |= DEBUG_FORMAT_USER_SMALL_FULL_MEMORY;
    flags |= DEBUG_FORMAT_USER_SMALL_HANDLE_DATA;
    flags |= DEBUG_FORMAT_USER_SMALL_PROCESS_THREAD_DATA;
    flags |= DEBUG_FORMAT_USER_SMALL_THREAD_INFO;
    flags |= DEBUG_FORMAT_USER_SMALL_CODE_SEGMENTS;
    flags |= DEBUG_FORMAT_USER_SMALL_FULL_MEMORY_INFO;
    flags |= DEBUG_FORMAT_USER_SMALL_DATA_SEGMENTS;

    AbortIfFailed(GDebugClient->WriteDumpFile2("test.dmp", DEBUG_DUMP_SMALL, flags, "Comment!"));
#endif

    
    std::println("=======");
    std::println("Modules");
    std::println("=======");
    AbortIfFailed(GDebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT, "!pe", executeMode));
    std::println();

    return DEBUG_STATUS_GO;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::CreateThread(
    ULONG64 Handle,
    ULONG64 DataOffset,
    ULONG64 StartOffset)
{
    std::println("CreateThread: {:016X} DataOffset: {:016X} StartOffset: {:016X}",
        Handle,
        DataOffset,
        StartOffset);

    return DEBUG_STATUS_GO;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::ExitThread(
    ULONG ExitCode)
{
    std::println("ExitThread: {}", ExitCode);

    return DEBUG_STATUS_GO;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::ExitProcess(
    ULONG ExitCode)
{
    std::println("ExitProcess: {}", ExitCode);

    return DEBUG_STATUS_GO;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::LoadModule(
    ULONG64 ImageFileHandle,
    ULONG64 BaseOffset,
    ULONG ModuleSize,
    PCSTR ModuleName,
    PCSTR ImageName,
    ULONG CheckSum,
    ULONG TimeDateStamp)
{
    std::println("LoadModule: {:016X} BaseOffset: {:016X} ModuleSize: {} ModuleName: {} ImageName: {} CheckSum: {} TimeDateStamp: {}",
        ImageFileHandle,
        BaseOffset,
        ModuleSize,
        ModuleName,
        ImageName,
        CheckSum,
        TimeDateStamp);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::UnloadModule(
    PCSTR ImageBaseName,
    ULONG64 BaseOffset)
{
    std::println("UnloadModule: {} BaseOffset: {:016X}",
        ImageBaseName,
        BaseOffset);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::SystemError(
    ULONG Error,
    ULONG Level)
{
    std::println("SystemError: {} Level: {}", Error, Level);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::SessionStatus(
    ULONG Status)
{
    std::println("SessionStatus: {}", Status);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::ChangeDebuggeeState(
    ULONG Flags,
    ULONG64 Argument)
{
    std::println("ChangeDebuggeeState: {} Argument: {:016X}",
        Flags,
        Argument);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::ChangeEngineState(
    ULONG Flags,
    ULONG64 Argument)
{
    std::println("ChangeEngineState: {} Argument: {:016X}",
        Flags,
        Argument);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::ChangeSymbolState(
    ULONG Flags,
    ULONG64 Argument)
{
    std::println("ChangeSymbolState: {} Argument: {:016X}",
        Flags,
        Argument);

    return S_OK;
}

COM_DECLSPEC_NOTHROW STDMETHODIMP MyCallback::CreateProcessW(
    ULONG64 ImageFileHandle,
    ULONG64 Handle,
    ULONG64 BaseOffset,
    ULONG ModuleSize,
    PCSTR ModuleName,
    PCSTR ImageName,
    ULONG CheckSum,
    ULONG TimeDateStamp,
    ULONG64 InitialThreadHandle,
    ULONG64 ThreadDataOffset,
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

    return S_OK;
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
        MyOutput output{GDebugClient};
        MyCallback callback{};

        std::println("DebuggerPid: {}, ClientPid: {}",
            GetCurrentProcessId(),
            options.ProcessId.value());

        AbortIfFailed(DebugCreate(IID_PPV_ARGS(GDebugClient.GetAddressOf())));

        AbortIfFailed(GDebugClient->QueryInterface(IID_PPV_ARGS(GDebugAdvanced.GetAddressOf())));

        AbortIfFailed(GDebugClient->QueryInterface(IID_PPV_ARGS(GDebugControl.GetAddressOf())));
        // AbortIfFailed(GDebugControl->AddEngineOptions(DEBUG_ENGOPT_INITIAL_BREAK));

        AbortIfFailed(GDebugClient->QueryInterface(IID_PPV_ARGS(GDebugRegisters.GetAddressOf())));
        AbortIfFailed(GDebugClient->QueryInterface(IID_PPV_ARGS(GDebugSymbols.GetAddressOf())));

        AbortIfFailed(GDebugClient->SetOutputCallbacks(&output));
        AbortIfFailed(GDebugClient->SetEventCallbacks(&callback));

        AbortIfFailed(GDebugClient->AttachProcess(0, pid, DEBUG_ATTACH_INVASIVE_RESUME_PROCESS));
        std::println("Attached to process: {}", pid);


        //
        // Here the application is running. We need to wait for the events.
        //

        bool finished = false;

        while (true)
        {

            std::println("Wait for event");
            if (FAILED(GDebugControl->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE)))
            {
                std::println("No more events");
                break;
            }

            ULONG status{};
            AbortIfFailed(GDebugControl->GetExecutionStatus(&status));
            std::println("Status: {}", status);

            if (status == DEBUG_STATUS_BREAK)
            {
                AbortIfFailed(GDebugControl->SetExecutionStatus(DEBUG_STATUS_GO));
            }
            else if (status == DEBUG_STATUS_NO_DEBUGGEE)
            {
                finished = true;
            }

            if (finished)
            {
                std::println("Finished 2");
                break;
            }
        }

        ULONG status{};
        AbortIfFailed(GDebugControl->GetExecutionStatus(&status));
        std::println("Status: {}", status);

        AbortIfFailed(GDebugClient->EndSession(DEBUG_END_ACTIVE_TERMINATE));


        GDebugRegisters = nullptr;
        GDebugControl = nullptr;
        GDebugAdvanced = nullptr;
        GDebugSymbols = nullptr;
        GDebugClient = nullptr;

        return 2137;
    }

    return 0;
}
