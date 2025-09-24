#pragma once
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"

namespace Anemone
{
    class WindowsEtwTraceListener final : public TraceListener
    {
    public:
        WindowsEtwTraceListener();

        ~WindowsEtwTraceListener() override;

        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
    };
}
