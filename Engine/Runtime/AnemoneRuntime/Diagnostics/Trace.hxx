#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone::Diagnostics
{
    enum class TraceLevel : uint8_t
    {
        /// @brief Most detailed messages.
        Verbose = 0,

        /// @brief Messages useful for debugging.
        Debug = 1,

        /// @brief General flow of the application.
        Information = 2,

        /// @brief Abnormal or unexpected events.
        Warning = 3,

        /// @brief General errors.
        Error = 4,

        /// @brief Critical unrecoverable errors.
        Critical = 5,

        /// @brief
        None = 6,
    };
}

namespace Anemone::Diagnostics::Internal
{
#if ANEMONE_BUILD_SHIPPING
    inline constexpr TraceLevel GTraceLevel = TraceLevel::Error;
#elif ANEMONE_CONFIG_DEBUG
    inline constexpr TraceLevel GTraceLevel = TraceLevel::Verbose;
#else
    inline constexpr TraceLevel GTraceLevel = TraceLevel::Warning;
#endif
}

namespace Anemone::Diagnostics
{
    class Trace;

    class TraceListener
        : private IntrusiveListNode<TraceListener, Trace>
    {
        friend struct IntrusiveList<TraceListener, Trace>;

    public:
        TraceListener() = default;

        TraceListener(TraceListener const&) = delete;

        TraceListener(TraceListener&&) = delete;

        TraceListener& operator=(TraceListener const&) = delete;

        TraceListener& operator=(TraceListener&&) = delete;

        virtual ~TraceListener() = default;

    public:
        /// @brief Writes a line to the trace listener.
        /// @param level    The level of the message.
        /// @param message  The message to write.
        /// @param size     The length of the message.
        ///
        /// @note Message is null-terminated. Size parameter specifies the length of the message, not including the null-terminator.
        virtual void WriteLine(TraceLevel level, const char* message, size_t size) = 0;

        virtual void Flush()
        {
        }
    };
}

namespace Anemone::Diagnostics
{
    class RUNTIME_API Trace final
    {
    private:
        IntrusiveList<TraceListener, Trace> _listeners{};
        ReaderWriterLock _lock{};

    public:
        Trace();

        Trace(Trace const&) = delete;

        Trace(Trace&&) = delete;

        Trace& operator=(Trace const&) = delete;

        Trace& operator=(Trace&&) = delete;

        ~Trace();

    public:
        void AddListener(TraceListener& listener);

        void RemoveListener(TraceListener& listener);

        void WriteLineFormatted(TraceLevel level, std::string_view format, fmt::format_args args);

        template <typename... Args>
        void WriteLine(TraceLevel level, std::string_view format, Args const&... args)
        {
            this->WriteLineFormatted(level, format, fmt::make_format_args(args...));
        }

        void Flush();
    };

    extern RUNTIME_API UninitializedObject<Trace> GTrace;
}

#define AE_TRACE(level, format, ...) \
    do \
    { \
        if constexpr (::Anemone::Diagnostics::TraceLevel::level >= ::Anemone::Diagnostics::Internal::GTraceLevel) \
        { \
            ::Anemone::Diagnostics::GTrace->WriteLine(::Anemone::Diagnostics::TraceLevel::level, format __VA_OPT__(, ) __VA_ARGS__); \
        } \
    } while (false)
