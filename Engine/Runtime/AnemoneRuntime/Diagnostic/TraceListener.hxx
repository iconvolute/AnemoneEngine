#pragma once
#include "AnemoneRuntime/Diagnostic/TraceLevel.hxx"
#include "AnemoneRuntime/Intrusive.hxx"

namespace Anemone::Diagnostic
{
    class Trace;

    class TraceListener : public IntrusiveListNode<TraceListener, Trace>
    {
        friend struct IntrusiveList<TraceListener, Trace>;

    public:
        TraceListener() = default;
        TraceListener(TraceListener const&) = default;
        TraceListener(TraceListener&&) = default;
        TraceListener& operator=(TraceListener const&) = default;
        TraceListener& operator=(TraceListener&&) = default;
        virtual ~TraceListener() = default;

    public:
        virtual void WriteLine(TraceLevel level, std::string_view message) = 0;

        virtual void Flush()
        {
        }
    };
}
