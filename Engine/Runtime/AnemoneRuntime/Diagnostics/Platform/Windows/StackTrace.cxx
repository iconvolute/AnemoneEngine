#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS && ANEMONE_FEATURE_STACKTRACE

#include "AnemoneRuntime/Diagnostics/Platform/Windows/DebugEngine.hxx"


#include <fmt/format.h>

#include <bit>
#include <iterator>

namespace Anemone::Diagnostics
{
    namespace
    {
        inline constexpr DWORD MaxFramesCount = 128;
    }
}

#endif

namespace Anemone::Diagnostics
{
    void GetCurrentStackTrace(
        FunctionRef<void(void* address)> callback)
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

    void GetCurrentStackTrace(
        FunctionRef<void(void* address, std::string_view functionName)> callback)
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

        DebugEngine& debugEngine = DebugEngine::Get();

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
