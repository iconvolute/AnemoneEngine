#include "AnemoneRuntime/Diagnostic/Runtime.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputTraceListener.hxx"

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
    static UninitializedObject<StandardOutputTraceListener> GStandardOutputTraceListener{};

    void InitializeRuntime(RuntimeInitializeContext& context)
    {
        (void)context;

        GTrace.Create();

        // if (context.UseStandardOutput)
        {
            GStandardOutputTraceListener.Create();
            GTrace->AddListener(GStandardOutputTraceListener.Get());
        }

#if ANEMONE_PLATFORM_ANDROID
        GAndroidLogTraceListener.Create();
#endif
    }

    void FinalizeRuntime(RuntimeFinalizeContext& context)
    {
        (void)context;

        // if (context.UseStandardOutput)
        {
            GTrace->RemoveListener(GStandardOutputTraceListener.Get());
            GStandardOutputTraceListener.Destroy();
        }

#if ANEMONE_PLATFORM_ANDROID
        GAndroidLogTraceListener.Destroy();
#endif

        GTrace.Destroy();
    }
}
