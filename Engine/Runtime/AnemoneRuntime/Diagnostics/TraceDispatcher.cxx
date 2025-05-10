#include "AnemoneRuntime/Diagnostics/TraceDispatcher.hxx"

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
