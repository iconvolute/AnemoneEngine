#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#if ANEMONE_FEATURE_STACKTRACE_SYMBOLS && ANEMONE_FEATURE_STACKTRACE

#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>

#endif

namespace Anemone::Diagnostics
{
    void Debug::GetCurrentStackTrace(
        FunctionRef<void(void* address)> callback)
    {
#if ANEMONE_FEATURE_STACKTRACE

        unw_context_t context{};
        unw_cursor_t cursor{};

        if (unw_getcontext(&context) != 0)
        {
            return;
        }

        if (unw_init_local(&cursor, &context) != 0)
        {
            return;
        }

        while (unw_step(&cursor) > 0)
        {
            unw_word_t pc{};
            unw_get_reg(&cursor, UNW_REG_IP, &pc);

            if (pc == 0)
            {
                break;
            }

            void* address = reinterpret_cast<void*>(pc);

            callback(address);
        }

#endif
    }

    void Debug::GetCurrentStackTrace(
        FunctionRef<void(void* address, std::string_view symbol)> callback)
    {
#if ANEMONE_FEATURE_STACKTRACE

        unw_context_t context{};
        unw_cursor_t cursor{};

        if (unw_getcontext(&context) != 0)
        {
            return;
        }

        if (unw_init_local(&cursor, &context) != 0)
        {
            return;
        }

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
                    int status{};
                    char* demangled = abi::__cxa_demangle(mangled.data(), nullptr, nullptr, &status);

                    if (status == 0)
                    {
                        fmt::format_to(std::back_inserter(current), "{}+{:#x}", demangled, offset);
                    }
                    else
                    {
                        // Fallback to mangled name
                        fmt::format_to(std::back_inserter(current), "{}+{:#x}", mangled.data(), offset);
                    }

                    if (demangled != nullptr)
                    {
                        free(demangled);
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
                    // Failed to get symbol name.
                    fmt::format_to(std::back_inserter(current), "<unknown>");
                    break;
                }
            }

            std::string_view const symbol{current.data(), current.size()};

            callback(address, symbol);
#else

            callback(address, "<unknown>");

#endif
        }
#endif
    }
}
