#pragma once
#include "AnemoneDiagnostics/TraceListener.hxx"

namespace Anemone
{
    class RUNTIME_API TraceDispatcher : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};
        IntrusiveList<TraceListener> _listeners{};

    public:
        void TraceEvent(TraceLevel level, const char* message, size_t size) override;

        void Flush() override;

        void Register(TraceListener& listener);

        void Unregister(TraceListener& listener);
    };
}
