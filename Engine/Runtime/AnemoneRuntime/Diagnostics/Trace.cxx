#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <utility>

namespace Anemone::Diagnostics
{
    void TraceDispatcher::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        SharedLock scope{this->_lock};

        this->_listeners.ForEach([&](TraceListener& listener)
        {
            listener.TraceEvent(level, message, size);
        });
    }

    void TraceDispatcher::Flush()
    {
        SharedLock scope{this->_lock};

        this->_listeners.ForEach([](TraceListener& listener)
        {
            listener.Flush();
        });
    }

    void TraceDispatcher::Register(TraceListener& listener)
    {
        UniqueLock scope{this->_lock};
        this->_listeners.PushBack(&listener);
    }

    void TraceDispatcher::Unregister(TraceListener& listener)
    {
        UniqueLock scope{this->_lock};
        this->_listeners.Remove(&listener);
    }
}

namespace Anemone::Diagnostics
{
    static UninitializedObject<TraceDispatcher> GTraceDispatcher{};

    TraceDispatcher& GetTraceDispatcher()
    {
        return *GTraceDispatcher;
    }
}

namespace Anemone::Internal
{
    extern void InitializeTrace()
    {
        ::Anemone::Diagnostics::GTraceDispatcher.Create();
    }

    extern void FinalizeTrace()
    {
        ::Anemone::Diagnostics::GTraceDispatcher.Destroy();
    }
}
