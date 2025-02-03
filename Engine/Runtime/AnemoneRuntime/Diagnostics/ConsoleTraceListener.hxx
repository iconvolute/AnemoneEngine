#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"

namespace Anemone
{
    class ConsoleTraceListener final
        : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
        void Flush() override;
    };
}
