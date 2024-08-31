#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Windows/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputLogListener.hxx"

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
    class EtwLogListener : public LogListener
    {
    public:
        void Log(Diagnostic::LogLevel level, std::string_view message) override
        {
            switch (level)  // NOLINT(clang-diagnostic-switch-enum)
            {
            case LogLevel::Fatal:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                    TraceLoggingString(message.data(), "Message"));
                break;

            case LogLevel::Error:
                TraceLoggingWrite(GTraceLoggingProvider,
                    "Fatal",
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingString(message.data(), "Message"));
                break;

            case LogLevel::Warning:
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

            Diagnostic::AddLogListener(this);
        }

        ~EtwLogListener()
        {
            Diagnostic::RemoveLogListener(this);

            TraceLoggingUnregister(GTraceLoggingProvider);
        }
    };

    class DebugOutputLogListener : public LogListener
    {
    public:
        void Log(Diagnostic::LogLevel level, std::string_view message) override
        {
            (void)level;
            Platform::win32_string_buffer<wchar_t, 512> buffer{};
            Platform::win32_WidenString(buffer, message);
            OutputDebugStringW(buffer.data());
        }

        void Flush() override
        {
        }

        DebugOutputLogListener()
        {
            Diagnostic::AddLogListener(this);
        }

        ~DebugOutputLogListener() override
        {
            Diagnostic::RemoveLogListener(this);
        }
    };

    static UninitializedObject<DebugOutputLogListener> GDebugOutputLogListener{};
    static UninitializedObject<StandardOutputLogListener> GStandardOutputLogListener{};
    //static UninitializedObject<EtwLogListener> GEtwLogListener{};

    void InitializeStatic()
    {
        GDebugOutputLogListener.Create();
        GStandardOutputLogListener.Create();
        //GEtwLogListener.Create();
    }

    void ShutdownStatic()
    {
        //GEtwLogListener.Destroy();
        GStandardOutputLogListener.Destroy();
        GDebugOutputLogListener.Destroy();
    }
}
