#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/SpinWait.hxx"
#include "AnemoneRuntime/Threading/Platform/Unix/UnixThread.hxx"
#include "AnemoneRuntime/Interop/Linux/Error.hxx"
#include "AnemoneRuntime/Interop/Linux/Process.hxx"
#include "AnemoneRuntime/Interop/Linux/Threading.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"

#include <cmath>
#include <utility>

namespace Anemone
{
    namespace
    {
        constexpr int ConvertThreadPriority(ThreadPriority priority) noexcept
        {
            switch (priority)
            {
            case ThreadPriority::TimeCritical:
            case ThreadPriority::Highest:
                return 30;
            case ThreadPriority::AboveNormal:
                return 25;
            case ThreadPriority::Normal:
                return 15;
            case ThreadPriority::BelowNormal:
                return 5;
            case ThreadPriority::Lower:
                return 14;
            case ThreadPriority::Lowest:
                return 1;
            }

            return 5;
        }
    }

    void* UnixThread::EntryPoint(void* param)
    {
        AE_ASSERT(param);

        StartupContext& context = *static_cast<StartupContext*>(param);

        UnixThread* self = context.thread.load(std::memory_order::acquire);
        AE_ASSERT(self != nullptr);
        
        self->_handle.Attach(pthread_self());
        self->_id = ThreadId{static_cast<uintptr_t>(Interop::Linux::GetThreadId())};

        // Set thread name
        if (context.start->Name)
        {
            Interop::string_buffer<char, 64> name{*context.start->Name};
            pthread_setname_np(self->_handle.Get(), name.c_str());
        }

        // Set thread priority
        if (context.start->Priority)
        {
            sched_param sched{};
            int32_t policy = SCHED_RR;

            if (pthread_getschedparam(self->_handle.Get(), &policy, &sched) == 0)
            {
                sched.sched_priority = ConvertThreadPriority(*context.start->Priority);

                pthread_setschedparam(self->_handle.Get(), policy, &sched);
            }
        }

        context.initialized.store(true, std::memory_order::release);

        self->_runnable->Run();

        self->ReleaseReference();

        pthread_exit(nullptr);
        return nullptr;
    }

    UnixThread::~UnixThread()
    {
        if (this->IsJoinable())
        {
            AE_PANIC("Destroying joinable thread. Did you forget to call Join or Detach?");
        }
    }

    ThreadId UnixThread::Id() const
    {
        AE_ASSERT(this->_handle);
        return this->_id;
    }

    void UnixThread::Join()
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(this->_handle.Get() != pthread_self());

        AE_UNIX_HANDLE_RESULT(pthread_join(this->_handle.Get(), nullptr));

        this->_handle = {};
        this->_id = {};
    }

    bool UnixThread::IsJoinable() const
    {
        return this->_handle.IsValid();
    }

    void UnixThread::Detach()
    {
        AE_ASSERT(this->_handle);

        AE_UNIX_HANDLE_RESULT(pthread_detach(this->_handle.Get()));

        this->_handle = {};
        this->_id = {};
    }

    Reference<UnixThread> UnixThread::Start(ThreadStart const& start)
    {
        if (not start.Callback)
        {
            AE_PANIC("Cannot create thread without runnable object");
        }

        Reference<UnixThread> result{new UnixThread{}};

        if (result)
        {
            // Bump refcount so the thread can hold a reference to itself.
            result->AcquireReference();

            // Store runnable object.
            result->_runnable = start.Callback;
            
            StartupContext context{
                .thread = result.Get(),
                .start = &start,
            };

            // Initialize thread attributes.
            pthread_attr_t attr{};

            AE_UNIX_HANDLE_RESULT(pthread_attr_init(&attr));

            if (start.StackSize)
            {
                size_t const pageSize = static_cast<size_t>(sysconf(_SC_PAGESIZE));
                size_t const stackSize = std::max<size_t>(*start.StackSize, PTHREAD_STACK_MIN);
                size_t const alignedStackSize = Bitwise::AlignUp(stackSize, pageSize);
                AE_UNIX_HANDLE_RESULT(pthread_attr_setstacksize(&attr, alignedStackSize));
            }

            AE_UNIX_HANDLE_RESULT(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));

            AE_UNIX_HANDLE_RESULT(pthread_create(result->_handle.GetAddressOf(), &attr, &UnixThread::EntryPoint, &context));

            AE_UNIX_HANDLE_RESULT(pthread_attr_destroy(&attr));

            WaitForCompletion(context.initialized);

            if (not result->_handle)
            {
                AE_PANIC("Failed to spawn thread");
            }

        }

        return result;
    }
}

namespace Anemone
{
    Reference<Thread> Thread::Start(ThreadStart const& threadStart)
    {
        return UnixThread::Start(threadStart);
    }
}
