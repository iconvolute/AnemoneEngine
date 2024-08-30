#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"

#if ANEMONE_FEATURE_STACKTRACE

ANEMONE_EXTERNAL_HEADERS_BEGIN

#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>

ANEMONE_EXTERNAL_HEADERS_END

#endif

namespace Anemone::Diagnostic
{
    anemone_noinline bool GetCurrentStackTrace(
        FunctionRef<bool(StackTraceEntry const& entry)> callback) noexcept
    {
#if ANEMONE_FEATURE_STACKTRACE

        unw_context_t context{};
        unw_cursor_t cursor{};

        if (unw_getcontext(&context) != 0)
        {
            return false;
        }

        if (unw_init_local(&cursor, &context) != 0)
        {
            return false;
        }

        size_t index = 0;

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS

        fmt::memory_buffer mangled{};
        fmt::memory_buffer current{};

#endif

        while (unw_step(&cursor) > 0)
        {
            unw_word_t pc{};

            unw_get_reg(&cursor, UNW_REG_IP, &pc);

            if (pc == 0)
            {
                break;
            }

            void* address = reinterpret_cast<void*>(pc);


#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS

            current.clear();
            mangled.clear();

            while (true)
            {
                unw_word_t offset{};
                int const rc = unw_get_proc_name(&cursor, mangled.data(), mangled.size(), &offset);

                if (rc == 0)
                {
                    // Try to demangle name
                    int status{};

                    char* demangled = abi::__cxa_demangle(mangled.data(), nullptr, nullptr, &status);

                    if (status == 0)
                    {
                        fmt::format_to(std::back_inserter(current), "{}+{:#x}", demangled, offset);
                    }
                    else
                    {
                        // Cannot demangle.
                        fmt::format_to(std::back_inserter(current), "{}+{:#x}", mangled.data(), offset);
                    }

                    if (demangled != nullptr)
                    {
                        std::free(demangled);
                    }

                    break;
                }
                else if (rc == -UNW_ENOMEM)
                {
                    // Get some more space and retry
                    mangled.resize(mangled.size() + 0x100);
                }
                else
                {
                    // Unknown error - cannot get proc name
                    fmt::format_to(std::back_inserter(current), "<unknown>");
                    break;
                }
            }

            if (!callback(StackTraceEntry{index, address, std::string_view{current.data(), current.size()}}))
            {
                break;
            }

#else

            if (!callback(StackTraceEntry{index, address, "<unknown>"}))
            {
                break;
            }

#endif
            ++index;
        }

#else

        (void)callback;

#endif

        return true;
    }
}
