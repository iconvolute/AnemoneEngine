#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Intrusive.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostics/TraceLevel.hxx"

namespace Anemone
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
        virtual void TraceEvent(TraceLevel level, const char* message, size_t size) = 0;
        virtual void Flush() { }
    };
}
