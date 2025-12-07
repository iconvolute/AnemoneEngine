#pragma once
#include "AnemoneRuntime.Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime.Storage/FileOutputStream.hxx"

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API FileTraceListener final : public TraceListener
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
