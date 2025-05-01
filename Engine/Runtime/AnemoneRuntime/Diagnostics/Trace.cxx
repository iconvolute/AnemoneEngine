#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/TraceListener.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <iterator>
#include <utility>
#include <array>

namespace Anemone::Diagnostics
{
    static constexpr std::array TraceLevelMarks = []()
    {
        return std::array{
            'V',
            'D',
            'I',
            'W',
            'E',
            'F',
            'N',
        };
    }();

    static constexpr char GetCharacter(TraceLevel level)
    {
        auto const index = std::to_underlying(level);
        if (index < TraceLevelMarks.size())
        {
            return TraceLevelMarks[index];
        }

        return 'N';
    }

    class GlobalTraceListener final : public TraceListener
    {
    private:
        ReaderWriterLock _lock{};
        IntrusiveList<TraceListener> _listeners{};

    public:
        void Event(TraceLevel level, const char* message, size_t size) override
        {
            SharedLock scope{this->_lock};

            this->_listeners.ForEach([&](TraceListener& listener)
            {
                listener.Event(level, message, size);
            });
        }

        void Flush() override
        {
            SharedLock scope{this->_lock};

            this->_listeners.ForEach([](TraceListener& listener)
            {
                listener.Flush();
            });
        }

        void Register(TraceListener& listener)
        {
            UniqueLock scope{this->_lock};
            this->_listeners.PushBack(&listener);
        }

        void Unregister(TraceListener& listener)
        {
            UniqueLock scope{this->_lock};
            this->_listeners.Remove(&listener);
        }
    };

    static UninitializedObject<GlobalTraceListener> GTraceListener{};

    RUNTIME_API void RegisterGlobalTraceListener(TraceListener& listener)
    {
        GTraceListener->Register(listener);
    }

    RUNTIME_API void UnregisterGlobalTraceListener(TraceListener& listener)
    {
        GTraceListener->Unregister(listener);
    }

    RUNTIME_API TraceListener& GetGlobalTraceListener()
    {
        return *GTraceListener;
    }

    RUNTIME_API void FlushTraceListeners()
    {
        GTraceListener->Flush();
    }

    RUNTIME_API void TraceMessageFormatted(TraceLevel level, std::string_view format, fmt::format_args args)
    {
        TraceMessageFormatted(*GTraceListener, level, format, args);
    }

    RUNTIME_API void TraceMessageFormatted(TraceListener& listener, TraceLevel level, std::string_view format, fmt::format_args args)
    {
        fmt::memory_buffer buffer{};

        auto out = std::back_inserter(buffer);
        (*out++) = '[';
        (*out++) = GetCharacter(level);
        (*out++) = ']';
        (*out++) = ' ';
        out = fmt::vformat_to(out, format, args);

        const char* message = buffer.data();
        size_t const size = buffer.size();

        (*out) = '\0';

        listener.Event(level, message, size);
    }

}

namespace Anemone::Internal
{
    extern void InitializeTrace()
    {
        ::Anemone::Diagnostics::GTraceListener.Create();
    }

    extern void FinalizeTrace()
    {
        ::Anemone::Diagnostics::GTraceListener.Destroy();
    }
}
