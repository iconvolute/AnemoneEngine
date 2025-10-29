#include "AnemoneDiagnostics/Platform/Windows/WindowsEtwTraceListener.hxx"

#include <winmeta.h>
#include <TraceLoggingProvider.h>

// C0B8BB2A-7E1E-5A0B-0ACD-3EE6187895ED
// https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/
TRACELOGGING_DEFINE_PROVIDER(
    gWindowsEtwTraceProvider,
    "AnemoneEngine.Runtime",
    (0xC0B8BB2A, 0x7E1E, 0x5A0B, 0x0A, 0xCD, 0x3E, 0xE6, 0x18, 0x78, 0x95, 0xED));

namespace Anemone
{
    WindowsEtwTraceListener::WindowsEtwTraceListener()
    {
        TraceLoggingRegister(gWindowsEtwTraceProvider);
    }

    WindowsEtwTraceListener::~WindowsEtwTraceListener()
    {
        TraceLoggingUnregister(gWindowsEtwTraceProvider);
    }

    void WindowsEtwTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        switch (level)
        {
        case TraceLevel::Verbose:
        case TraceLevel::Debug:
            TraceLoggingWrite(
                gWindowsEtwTraceProvider,
                "Log",
                TraceLoggingLevel(WINEVENT_LEVEL_VERBOSE),
                TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
            break;

        case TraceLevel::Information:
            TraceLoggingWrite(
                gWindowsEtwTraceProvider,
                "Log",
                TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
            break;

        case TraceLevel::Warning:
            TraceLoggingWrite(
                gWindowsEtwTraceProvider,
                "Log",
                TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));

            break;
        case TraceLevel::Error:
            TraceLoggingWrite(
                gWindowsEtwTraceProvider,
                "Log",
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
            break;

        case TraceLevel::Fatal:
            TraceLoggingWrite(
                gWindowsEtwTraceProvider,
                "Log",
                TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
            break;

        case TraceLevel::None:
            break;
        }
    }
}
