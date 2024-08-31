#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"

namespace Anemone::Diagnostic
{
    class StandardOutputTraceListener final
        : public Diagnostic::TraceListener
    {
    private:
        FILE* m_output = stdout;

    public:
        StandardOutputTraceListener()
        {
            GTrace->AddListener(*this);
        }

        ~StandardOutputTraceListener() override
        {
            GTrace->RemoveListener(*this);
        }


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
