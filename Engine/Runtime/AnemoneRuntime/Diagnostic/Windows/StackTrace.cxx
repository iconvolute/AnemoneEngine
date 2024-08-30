#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <DbgHelp.h>
#include <initguid.h>
#include <DbgEng.h>
#include <Shlwapi.h>

#include <iterator>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Diagnostic::Private
{
    class [[nodiscard]] DebugEngine final
    {
    private:
        inline static SRWLOCK s_DebugClientLock = SRWLOCK_INIT;
        inline static IDebugClient* s_DebugClient = nullptr;
        inline static IDebugSymbols* s_DebugSymbols = nullptr;
        inline static IDebugControl* s_DebugControl = nullptr;
        inline static HMODULE s_DebugModule = nullptr;
        inline static bool s_DebugAttached = false;
        inline static bool s_DebugTriedInitialize = false;

    public:
        DebugEngine() noexcept
        {
            AcquireSRWLockExclusive(&s_DebugClientLock);

            Initialize();
        }

        ~DebugEngine() noexcept
        {
            ReleaseSRWLockExclusive(&s_DebugClientLock);
        }

        DebugEngine(DebugEngine const&) = delete;
        DebugEngine(DebugEngine&&) = delete;
        DebugEngine& operator=(DebugEngine const&) = delete;
        DebugEngine& operator=(DebugEngine&&) = delete;

    public:
        [[nodiscard]] operator bool() const noexcept
        {
            return s_DebugAttached;
        }

    public:
        void Symbolize(
            void* address,
            fmt::memory_buffer& buffer) const noexcept
        {
            buffer.clear();

            ULONG64 displacement{};

            ULONG ulBufferCapacity{};

            HRESULT hr = S_FALSE;

            while (SUCCEEDED(hr))
            {
                hr = s_DebugSymbols->GetNameByOffset(
                    reinterpret_cast<uintptr_t>(address),
                    buffer.data(),
                    ulBufferCapacity,
                    &ulBufferCapacity,
                    &displacement);


                if (hr == S_OK || ulBufferCapacity == 0)
                {
                    break;
                }

                buffer.resize(ulBufferCapacity - 1);
            }

            if (FAILED(hr))
            {
                buffer.clear();
                fmt::format_to(std::back_inserter(buffer), "<unknown>");
            }
            else
            {
                fmt::format_to(std::back_inserter(buffer), "+{:#x}", displacement);
            }
        }

    private:
        static void Initialize() noexcept
        {
            if (!s_DebugTriedInitialize)
            {
                s_DebugTriedInitialize = true;

                if (std::atexit(Finalize) == 0)
                {
                    s_DebugModule = LoadLibraryExW(L"dbgeng.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

                    if (s_DebugModule != nullptr)
                    {
                        auto const fnDebugCreate = reinterpret_cast<decltype(&DebugCreate)>(std::bit_cast<void*>(GetProcAddress(s_DebugModule, "DebugCreate")));

                        if (fnDebugCreate != nullptr)
                        {
                            if (SUCCEEDED(fnDebugCreate(IID_IDebugClient, reinterpret_cast<void**>(&s_DebugClient))) &&
                                SUCCEEDED(s_DebugClient->QueryInterface(IID_IDebugSymbols, reinterpret_cast<void**>(&s_DebugSymbols))) &&
                                SUCCEEDED(s_DebugClient->QueryInterface(IID_IDebugControl, reinterpret_cast<void**>(&s_DebugControl))))
                            {
                                s_DebugAttached = SUCCEEDED(s_DebugClient->AttachProcess(0, GetCurrentProcessId(), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));

                                if (s_DebugAttached)
                                {
                                    std::ignore = s_DebugControl->WaitForEvent(0, INFINITE);
                                }

                                std::ignore = s_DebugSymbols->AddSymbolOptions(SYMOPT_CASE_INSENSITIVE |
                                    SYMOPT_UNDNAME |
                                    SYMOPT_DEFERRED_LOADS |
                                    SYMOPT_LOAD_LINES |
                                    SYMOPT_OMAP_FIND_NEAREST |
                                    SYMOPT_FAIL_CRITICAL_ERRORS |
                                    SYMOPT_AUTO_PUBLICS |
                                    SYMOPT_NO_PROMPTS);

                                std::ignore = s_DebugSymbols->RemoveSymbolOptions(SYMOPT_NO_CPP |
                                    SYMOPT_LOAD_ANYTHING |
                                    SYMOPT_NO_UNQUALIFIED_LOADS |
                                    SYMOPT_EXACT_SYMBOLS |
                                    SYMOPT_IGNORE_NT_SYMPATH |
                                    SYMOPT_PUBLICS_ONLY |
                                    SYMOPT_NO_PUBLICS |
                                    SYMOPT_NO_IMAGE_SEARCH);
                            }
                        }
                    }
                }
            }
        }

        static void Finalize() noexcept
        {
            AcquireSRWLockExclusive(&s_DebugClientLock);

            if (s_DebugClient != nullptr)
            {
                if (s_DebugAttached)
                {
                    (void)s_DebugClient->DetachProcesses();
                    s_DebugAttached = false;
                }

                s_DebugClient->Release();
                s_DebugClient = nullptr;
            }

            if (s_DebugControl != nullptr)
            {
                s_DebugControl->Release();
                s_DebugControl = nullptr;
            }

            if (s_DebugSymbols != nullptr)
            {
                s_DebugSymbols->Release();
                s_DebugSymbols = nullptr;
            }

            if (s_DebugModule != nullptr)
            {
                (void)FreeLibrary(s_DebugModule);
                s_DebugModule = nullptr;
            }

            s_DebugTriedInitialize = false;

            ReleaseSRWLockExclusive(&s_DebugClientLock);
        }
    };
}

#endif

namespace Anemone::Diagnostic
{
    bool GetCurrentStackTrace(
        FunctionRef<bool(StackTraceEntry const& entry)> callback) noexcept
    {
#if ANEMONE_FEATURE_STACKTRACE
        constexpr DWORD MaxFramesCount = 128;
        void* frames[MaxFramesCount];

        WORD const captured = CaptureStackBackTrace(
            1,
            static_cast<DWORD>(MaxFramesCount),
            frames,
            nullptr);

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS
        Private::DebugEngine engine{};

        fmt::memory_buffer nameBuffer{};

        for (size_t i = 0; i < captured; ++i)
        {
            engine.Symbolize(frames[i], nameBuffer);

            if (!callback(StackTraceEntry{i, frames[i], std::string_view{nameBuffer.data(), nameBuffer.size()}}))
            {
                break;
            }
        }

#else

        for (size_t i = 0; i < captured; ++i)
        {
            if (!callback(StackTraceEntry{i, frames[i], "<unknown>"}))
            {
                break;
            }
        }

#endif

#else

        (void)callback;

#endif
        return true;
    }
}
