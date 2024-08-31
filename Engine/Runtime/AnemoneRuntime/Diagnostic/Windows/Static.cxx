#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Windows/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

#include <winmeta.h>
#include <TraceLoggingProvider.h>

namespace Anemone::Diagnostic
{
    // {EE0B606D-9C16-43E9-AE90-1D1E8EE3F79B}
    TRACELOGGING_DEFINE_PROVIDER(
        GTraceLoggingProvider,
        "AnemoneEngine.Runtime",
        (0xee0b606d, 0x9c16, 0x43e9, 0xae, 0x90, 0x1d, 0x1e, 0x8e, 0xe3, 0xf7, 0x9b));
}

namespace Anemone::Diagnostic
{
    // https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/
    class EtwLogListener : public TraceListener
    {
    public:
        void WriteLine(TraceLevel level, std::string_view message) override
        {
            switch (level) // NOLINT(clang-diagnostic-switch-enum)
            {
            case TraceLevel::Critical:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                    TraceLoggingString(message.data(), "Message"));
                break;

            case TraceLevel::Error:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingString(message.data(), "Message"));
                break;

            case TraceLevel::Warning:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                    TraceLoggingString(message.data(), "Message"));
                break;

            default:
                break;
            }
        }

        void Flush() override
        {
        }

        EtwLogListener()
        {
            TraceLoggingRegister(GTraceLoggingProvider);

            GTrace->AddListener(*this);
        }

        ~EtwLogListener()
        {
            GTrace->RemoveListener(*this);

            TraceLoggingUnregister(GTraceLoggingProvider);
        }
    };

    class DebugOutputTraceListener : public TraceListener
    {
    public:
        void WriteLine(TraceLevel level, std::string_view message) override
        {
            (void)level;
            Platform::win32_string_buffer<wchar_t, 512> buffer{};
            Platform::win32_WidenString(buffer, message);
            OutputDebugStringW(buffer.data());
            OutputDebugStringW(L"\n");
        }

        void Flush() override
        {
        }

        DebugOutputTraceListener()
        {
            GTrace->AddListener(*this);
        }

        ~DebugOutputTraceListener() override
        {
            GTrace->RemoveListener(*this);
        }
    };
}

namespace Anemone::Diagnostic
{
    static UninitializedObject<DebugOutputTraceListener> GDebugOutputTraceListener{};
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};
    // static UninitializedObject<EtwLogListener> GEtwLogListener{};

    void InitializeStatic()
    {
        GDiagnosticStatic.Create();
        GTrace.Create();

        GDebugOutputTraceListener.Create();
        GStandardOutputTraceListener.Create();
        // GEtwLogListener.Create();
    }

    void ShutdownStatic()
    {
        // GEtwLogListener.Destroy();
        GStandardOutputTraceListener.Destroy();
        GDebugOutputTraceListener.Destroy();

        GTrace.Destroy();
        GDiagnosticStatic.Destroy();
    }
}
