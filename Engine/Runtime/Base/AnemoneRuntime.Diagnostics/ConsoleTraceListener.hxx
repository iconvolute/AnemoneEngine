#pragma once
#include "AnemoneRuntime.Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime.Threading/ReaderWriterLock.hxx"

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API ConsoleTraceListener final
        : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
        void Flush() override;
    };
}
