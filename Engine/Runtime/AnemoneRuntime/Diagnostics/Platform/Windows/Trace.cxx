#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Interop/Windows/Debugger.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <winmeta.h>
#include <TraceLoggingProvider.h>

#if !ANEMONE_BUILD_SHIPPING
namespace Anemone::Diagnostics
{
    namespace
    {
        class DebugTraceListener final : public TraceListener
        {
        private:
            CriticalSection m_lock{};

        public:
            void TraceEvent(TraceLevel level, const char* message, size_t size) override
            {
                (void)level;

                UniqueLock scope{this->m_lock};
                Interop::Windows::OutputDebugStringLengthA(message, size + 1);
                Interop::Windows::OutputDebugStringLengthA("\r\n", 3);
            }
        };

        UninitializedObject<DebugTraceListener> GDebugTraceListener{};
    }
}
#endif

namespace Anemone::Diagnostics
{
    namespace
    {
        // C0B8BB2A-7E1E-5A0B-0ACD-3EE6187895ED
        // https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/
        TRACELOGGING_DEFINE_PROVIDER(
            GWindowsEtwTraceProvider,
            "AnemoneEngine.Runtime",
            (0xC0B8BB2A, 0x7E1E, 0x5A0B, 0x0A, 0xCD, 0x3E, 0xE6, 0x18, 0x78, 0x95, 0xED));

        class EtwTraceListener final : public TraceListener
        {
        public:
            EtwTraceListener()
            {
                TraceLoggingRegister(GWindowsEtwTraceProvider);
            }

            ~EtwTraceListener() override
            {
                TraceLoggingUnregister(GWindowsEtwTraceProvider);
            }

            void TraceEvent(TraceLevel level, const char* message, size_t size) override
            {
                switch (level)
                {
                case TraceLevel::Verbose:
                case TraceLevel::Debug:
                    TraceLoggingWrite(
                        GWindowsEtwTraceProvider,
                        "Log",
                        TraceLoggingLevel(WINEVENT_LEVEL_VERBOSE),
                        TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                    break;

                case TraceLevel::Information:
                    TraceLoggingWrite(
                        GWindowsEtwTraceProvider,
                        "Log",
                        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                        TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                    break;

                case TraceLevel::Warning:
                    TraceLoggingWrite(
                        GWindowsEtwTraceProvider,
                        "Log",
                        TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                        TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));

                    break;
                case TraceLevel::Error:
                    TraceLoggingWrite(
                        GWindowsEtwTraceProvider,
                        "Log",
                        TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                        TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                    break;

                case TraceLevel::Fatal:
                    TraceLoggingWrite(
                        GWindowsEtwTraceProvider,
                        "Log",
                        TraceLoggingLevel(WINEVENT_LEVEL_CRITICAL),
                        TraceLoggingCountedUtf8String(message, static_cast<ULONG>(size), "Message"));
                    break;

                case TraceLevel::None:
                    break;
                }
            }
        };

        UninitializedObject<EtwTraceListener> GEtwTraceListener{};
    }
}

namespace Anemone::Internal
{
    extern void PlatformInitializeTrace()
    {
#if !ANEMONE_BUILD_SHIPPING
        Diagnostics::GDebugTraceListener.Create();
        Diagnostics::Trace::Get().Register(*Diagnostics::GDebugTraceListener);
#endif

        Diagnostics::GEtwTraceListener.Create();
        Diagnostics::Trace::Get().Register(*Diagnostics::GEtwTraceListener);
    }

    extern void PlatformFinalizeTrace()
    {
        if (Diagnostics::GEtwTraceListener)
        {
            Diagnostics::Trace::Get().Unregister(*Diagnostics::GEtwTraceListener);
            Diagnostics::GEtwTraceListener.Destroy();
        }

#if !ANEMONE_BUILD_SHIPPING
        if (Diagnostics::GDebugTraceListener)
        {
            Diagnostics::Trace::Get().Unregister(*Diagnostics::GDebugTraceListener);
            Diagnostics::GDebugTraceListener.Destroy();
        }
#endif
    }
}
