#pragma once
#include "AnemoneDiagnostics/TraceListener.hxx"
#include "AnemoneThreading/ReaderWriterLock.hxx"

namespace Anemone
{
    class RUNTIME_API ConsoleTraceListener final
        : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
        void Flush() override;
    };
}
