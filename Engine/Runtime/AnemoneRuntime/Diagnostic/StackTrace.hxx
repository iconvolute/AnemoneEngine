#pragma once
#include "AnemoneRuntime/FunctionRef.hxx"

#include <fmt/format.h>

namespace Anemone::Diagnostic
{
    struct StackTraceEntry final
    {
        size_t Index;
        void* Address;
        std::string_view Symbol;
    };

    RUNTIME_API bool GetCurrentStackTrace(FunctionRef<bool(StackTraceEntry const& entry)> callback) noexcept;
}

template <>
struct fmt::formatter<Anemone::Diagnostic::StackTraceEntry>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Anemone::Diagnostic::StackTraceEntry const& value, auto& context) const
    {
        return fmt::format_to(context.out(), "#{:02} {} {}", value.Index, value.Address, value.Symbol);
    }
};
