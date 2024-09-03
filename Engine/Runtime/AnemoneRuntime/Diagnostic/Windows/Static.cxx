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
                    TraceLoggingCountedUtf8String(message.data(), message.size(), "Message"));
                break;

            case TraceLevel::Error:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingCountedUtf8String(message.data(), message.size(), "Message"));
                break;

            case TraceLevel::Warning:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                    TraceLoggingCountedUtf8String(message.data(), message.size(), "Message"));
                break;

            default:
                break;
            }
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
            // https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw

            OutputDebugStringA(message.data());
            OutputDebugStringA("\r\n");
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
    // static UninitializedObject<EtwLogListener> GEtwLogListener{};

    void WindowsDiagnosticStatic::Initialize()
    {
        GenericDiagnosticStatic::Initialize();

        GDebugOutputTraceListener.Create();
        // GEtwLogListener.Create();
    }

    void WindowsDiagnosticStatic::Finalize()
    {
        // GEtwLogListener.Destroy();
        GDebugOutputTraceListener.Destroy();

        GenericDiagnosticStatic::Finalize();
    }
}
