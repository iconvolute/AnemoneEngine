#pragma once
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    class WindowsDebugTraceListener final : public TraceListener
    {
    private:
        CriticalSection m_lock{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
    };
}
