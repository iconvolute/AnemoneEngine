#pragma once
#include "AnemoneDiagnostics/TraceListener.hxx"
#include "AnemoneThreading/CriticalSection.hxx"

namespace Anemone
{
    class AndroidDebugTraceListener final : public TraceListener
    {
    private:
        CriticalSection m_lock{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
    };
}
