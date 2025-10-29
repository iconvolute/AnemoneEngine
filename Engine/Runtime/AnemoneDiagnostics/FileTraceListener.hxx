#pragma once
#include "AnemoneDiagnostics/TraceListener.hxx"
#include "AnemoneStorage/FileOutputStream.hxx"

namespace Anemone
{
    class RUNTIME_API FileTraceListener final : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};
        Reference<FileOutputStream> _stream{};

    public:
        explicit FileTraceListener(Reference<FileOutputStream> stream);

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;
        void Flush() override;
    };
}
