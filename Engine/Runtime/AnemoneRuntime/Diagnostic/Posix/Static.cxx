#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#if ANEMONE_PLATFORM_ANDROID

#include <android/log.h>

namespace Anemone::Diagnostic
{
    constexpr android_LogPriority ToAndroidLogPriority(TraceLevel level)
    {
        switch (level)
        {
        case TraceLevel::Critical:
            return ANDROID_LOG_FATAL;
        case TraceLevel::Error:
            return ANDROID_LOG_ERROR;
        case TraceLevel::Warning:
            return ANDROID_LOG_WARN;
        case TraceLevel::Info:
            return ANDROID_LOG_INFO;
        case TraceLevel::Verbose:
            return ANDROID_LOG_VERBOSE;
        default:
            return ANDROID_LOG_DEBUG;
        }
    }

    class AndroidLogTraceListener final
        : public Diagnostic::TraceListener
    {
    public:
        AndroidLogTraceListener()
        {
            GTrace->AddListener(*this);
        }

        ~AndroidLogTraceListener() override
        {
            GTrace->RemoveListener(*this);
        }

        void WriteLine(TraceLevel level, std::string_view message) override
        {
            __android_log_write(
                ToAndroidLogPriority(level),
                "Anemone", message.data());
        }
    };

    static UninitializedObject<AndroidLogTraceListener> GAndroidLogTraceListener;
}

#endif

namespace Anemone::Diagnostic
{
    void PosixDiagnosticStatic::Initialize()
    {
        GenericDiagnosticStatic::Initialize();

#if ANEMONE_PLATFORM_ANDROID
        GAndroidLogTraceListener.Create();
#endif
    }

    void PosixDiagnosticStatic::Finalize()
    {
#if ANEMONE_PLATFORM_ANDROID
        GAndroidLogTraceListener.Destroy();
#endif

        GenericDiagnosticStatic::Finalize();
    }
}
