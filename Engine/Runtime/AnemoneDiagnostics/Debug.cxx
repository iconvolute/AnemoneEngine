#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneBase/UninitializedObject.hxx"
#include "AnemoneThreading/CriticalSection.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneDiagnostics/Platform/Windows/WindowsDebug.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneDiagnostics/Platform/Linux/LinuxDebug.hxx"
#else
#error Not implemented
#endif

#include <iterator>

namespace Anemone
{
    namespace
    {
        UninitializedObject<CriticalSection> gDebugLock{};
    }

    void Debug::Initialize()
    {
        gDebugLock.Create();

        PlatformDebug::Initialize();
    }

    void Debug::Finalize()
    {
        PlatformDebug::Finalize();

        gDebugLock.Destroy();
    }

    bool Debug::ReportAssertionFormatted(
        std::source_location const& location,
        std::string_view expression,
        std::string_view format,
        fmt::format_args args)
    {
        UniqueLock lock{*gDebugLock};

        // TODO: It would be nice to have tracing handle lack of trace listeners instead???
        TraceDispatcher& dispatcher = Trace::Get();

        fmt::memory_buffer message{};
        fmt::vformat_to(std::back_inserter(message), format, args);

        dispatcher.TraceFatal("=== assertion failed ===");
        dispatcher.TraceFatal("location: {}:{}", location.file_name(), location.line());
        dispatcher.TraceFatal("expression: {}", expression);
        dispatcher.TraceFatal("message: {}", std::string_view{message.data(), message.size()});

        GetCurrentStackTrace([&](void* address, std::string_view name)
        {
            dispatcher.TraceFatal("{} {}", address, name);
        });

        dispatcher.Flush();

        // TODO: Re-enable once we will be sure that it also works on linux
#if false
        if (Debugger::Attach())
        {
            // Debugger is attached. Continue execution.
            return true;
        }
#endif

#if ANEMONE_BUILD_SHIPPING
        // Always execute breakpoint in shipping builds. It will propagate to the crash handler.
        return true;
#else
        // TODO: Add option to ignore assertions at runtime?
        return true;
#endif
    }

    void Debug::ReportPanicFormatted(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args)
    {
        UniqueLock lock{*gDebugLock};

        TraceDispatcher& dispatcher = Trace::Get();

        fmt::memory_buffer message{};
        fmt::vformat_to(std::back_inserter(message), format, args);

        dispatcher.TraceFatal("=== panic ===");
        dispatcher.TraceFatal("location: {}:{}", location.file_name(), location.line());
        dispatcher.TraceFatal("message: {}", std::string_view{message.data(), message.size()});

        GetCurrentStackTrace([&](void* address, std::string_view name)
        {
            dispatcher.TraceFatal("{} {}", address, name);
        });

        dispatcher.Flush();

        // TODO: Re-enable once we will be sure that it also works on linux
#if false
        Debugger::Attach();
#endif
    }
}
