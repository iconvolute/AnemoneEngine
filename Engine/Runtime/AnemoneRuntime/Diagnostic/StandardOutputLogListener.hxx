#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"

namespace Anemone::Diagnostic
{
    class StandardOutputLogListener final
        : public Diagnostic::LogListener
    {
    private:
        FILE* m_output = stdout;

    public:
        StandardOutputLogListener()
        {
            Diagnostic::AddLogListener(this);
        }

        ~StandardOutputLogListener() override
        {
            Diagnostic::RemoveLogListener(this);
        }


        void Log(Diagnostic::LogLevel level, std::string_view message) override
        {
            (void)level;
            fwrite(message.data(), message.size(), 1, this->m_output);
        }

        void Flush() override
        {
            fflush(this->m_output);
        }
    };
}
