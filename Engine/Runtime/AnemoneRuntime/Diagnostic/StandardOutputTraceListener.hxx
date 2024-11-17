#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone::Diagnostic
{
    class StandardOutputTraceListener final
        : public TraceListener
    {
    private:
        FILE* m_output = stdout;

    public:
        void WriteLine(TraceLevel level, std::string_view message) override
        {
            (void)level;
            fwrite(message.data(), message.size(), 1, this->m_output);
            fputc('\n', this->m_output);
        }

        void Flush() override
        {
            fflush(this->m_output);
        }
    };
}
