#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS && ANEMONE_FEATURE_STACKTRACE

#include <fmt/format.h>
#include <DbgHelp.h>
#include <initguid.h>
#include <DbgEng.h>
#include <Shlwapi.h>
#include <wrl/client.h>

#include <bit>
#include <iterator>

namespace Anemone
{
    namespace
    {
        inline constexpr DWORD MaxFramesCount = 128;

        class DebugEngineImpl final
        {
        private:
            Microsoft::WRL::ComPtr<IDebugClient> _client{};
            Microsoft::WRL::ComPtr<IDebugControl> _control{};
            Microsoft::WRL::ComPtr<IDebugSymbols> _symbols{};
            Interop::Windows::SafeSharedLibraryHandle _module{};
            bool _attached{};

        public:
            DebugEngineImpl()
            {
                this->_module.Attach(
                    LoadLibraryExW(
                        L"dbgeng.dll",
                        nullptr,
                        LOAD_LIBRARY_SEARCH_SYSTEM32));

                if (this->_module)
                {
                    auto const fnDebugCreate = reinterpret_cast<decltype(&DebugCreate)>(
                        std::bit_cast<void*>(
                            GetProcAddress(this->_module.Get(), "DebugCreate")));

                    if (fnDebugCreate)
                    {
                        if (SUCCEEDED(fnDebugCreate(IID_PPV_ARGS(this->_client.GetAddressOf()))) and
                            SUCCEEDED(this->_client->QueryInterface(IID_PPV_ARGS(this->_symbols.GetAddressOf()))) and
                            SUCCEEDED(this->_client->QueryInterface(IID_PPV_ARGS(this->_control.GetAddressOf()))))
                        {
                            this->_attached = SUCCEEDED(this->_client->AttachProcess(0, GetCurrentProcessId(), DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));

                            if (this->_attached)
                            {
                                (void)this->_control->WaitForEvent(0, INFINITE);
                            }

                            (void)this->_symbols->AddSymbolOptions(SYMOPT_CASE_INSENSITIVE |
                                SYMOPT_UNDNAME |
                                SYMOPT_DEFERRED_LOADS |
                                SYMOPT_LOAD_LINES |
                                SYMOPT_OMAP_FIND_NEAREST |
                                SYMOPT_FAIL_CRITICAL_ERRORS |
                                SYMOPT_AUTO_PUBLICS |
                                SYMOPT_NO_PROMPTS);

                            (void)this->_symbols->RemoveSymbolOptions(SYMOPT_NO_CPP |
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

            DebugEngineImpl(DebugEngineImpl const&) = delete;

            DebugEngineImpl(DebugEngineImpl&&) = delete;

            DebugEngineImpl& operator=(DebugEngineImpl const&) = delete;

            DebugEngineImpl& operator=(DebugEngineImpl&&) = delete;

            ~DebugEngineImpl()
            {
                if (this->_client)
                {
                    if (this->_attached)
                    {
                        this->_client->DetachProcesses();
                        this->_attached = false;
                    }
                }

                this->_client.Reset();
                this->_control.Reset();
                this->_symbols.Reset();
                this->_module.Reset();
            }

            static DebugEngineImpl& Get()
            {
                static DebugEngineImpl instance{};
                return instance;
            }

        public:
            void Symbolize(void* address, fmt::memory_buffer& result) const
            {
                result.clear();

                ULONG64 displacement{};
                ULONG ulBufferCapacity{};
                HRESULT hr = S_FALSE;

                while (SUCCEEDED(hr))
                {
                    hr = this->_symbols->GetNameByOffset(
                        reinterpret_cast<uintptr_t>(address),
                        result.data(),
                        ulBufferCapacity,
                        &ulBufferCapacity,
                        &displacement);

                    if ((hr == S_OK) or (ulBufferCapacity == 0))
                    {
                        break;
                    }

                    result.resize(ulBufferCapacity - 1uz);
                }

                if (FAILED(hr))
                {
                    result.clear();
                    fmt::format_to(std::back_inserter(result), "<unknown>");
                }
                else
                {
                    fmt::format_to(std::back_inserter(result), "+{:#x}", displacement);
                }
            }
        };
    }
}

#endif

namespace Anemone
{
    void Debug::GetCurrentStackTrace(FunctionRef<void(void* address)> callback)
    {
#if ANEMONE_FEATURE_STACKTRACE

        void* frames[MaxFramesCount];

        size_t processed = RtlCaptureStackBackTrace(
            1,
            MaxFramesCount,
            frames,
            nullptr);

        if (processed == 0)
        {
            // Retry if failed to capture stack trace.
            processed = RtlCaptureStackBackTrace(
                1,
                MaxFramesCount,
                frames,
                nullptr);

            if (processed == 0)
            {
                // Debug print about that. Do not use logger here.
                OutputDebugStringA("Failed to capture stack trace.\n");
            }
        }

        for (size_t i = 0; i < processed; ++i)
        {
            callback(frames[i]);
        }

#else

        (void)callback;

#endif
    }

    void Debug::GetCurrentStackTrace(FunctionRef<void(void* address, std::string_view symbol)> callback)
    {
#if ANEMONE_FEATURE_STACKTRACE

        void* frames[MaxFramesCount];

        size_t processed = RtlCaptureStackBackTrace(
            1,
            MaxFramesCount,
            frames,
            nullptr);

        if (processed == 0)
        {
            // Retry if failed to capture stack trace.
            processed = RtlCaptureStackBackTrace(
                1,
                MaxFramesCount,
                frames,
                nullptr);

            if (processed == 0)
            {
                // Debug print about that. Do not use logger here.
                OutputDebugStringA("Failed to capture stack trace.\n");
            }
        }

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS

        DebugEngineImpl& debugEngine = DebugEngineImpl::Get();

        fmt::memory_buffer line{};
        for (size_t i = 0; i < processed; ++i)
        {
            debugEngine.Symbolize(frames[i], line);

            callback(frames[i], std::string_view(line.data(), line.size()));
        }

#else

        for (size_t i = 0; i < processed; ++i)
        {
            callback(frames[i], "<unknown>");
        }

#endif

#endif
    }
}
