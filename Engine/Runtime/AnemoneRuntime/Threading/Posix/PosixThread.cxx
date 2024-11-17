#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <cmath>
#include <utility>

namespace Anemone::Private
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

    static void* ThreadEntryPoint(void* context)
    {
        if (context == nullptr)
        {
            AE_PANIC("Thread started without context.");
        }

        Runnable* const runnable = static_cast<Runnable*>(context);
        runnable->Run();

        pthread_exit(nullptr);
        return nullptr;
    }

}

namespace Anemone
{
    Thread::Thread()
        : m_native{}
    {
    }

    Thread::Thread(ThreadStart const& start)
        : m_native{}
    {
        if (start.Callback == nullptr)
        {
            AE_PANIC("Thread started without callback.");
        }

        pthread_attr_t attr{};

        if (int const rc = pthread_attr_init(&attr); rc != 0)
        {
            AE_PANIC("pthread_attr_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (start.StackSize)
        {
            if (int const rc = pthread_attr_setstacksize(&attr, *start.StackSize); rc != 0)
            {
                AE_PANIC("pthread_attr_init (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        if (int const rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); rc != 0)
        {
            AE_PANIC("pthread_attr_setdetachstate (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_create(&this->m_native.Handle, &attr, Private::ThreadEntryPoint, start.Callback); rc != 0)
        {
            AE_PANIC("pthread_create (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_attr_destroy(&attr); rc != 0)
        {
            AE_PANIC("pthread_attr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (this->m_native.Handle == 0)
        {
            AE_PANIC("Failed to start thread");
        }


        //
        // FIXME:
        //     Thread is started immediately in pthread_create. It is possible that the thread will
        //     finish before we set the name and priority.
        //
        //     In that case, either ignore errors in reported by further pthread calls or notify
        //     thread callback that we finished initialization process so it can continue.
        //
        //     This is not a problem in Windows implementation because thread is started in suspended
        //     state.
        //
        //     Alternatively we can set these properties in the thread callback.
        //

        if (start.Name)
        {
            std::string const name{*start.Name};
#if true
            pthread_setname_np(this->m_native.Handle, name.c_str());
#else
            if (int const rc = pthread_setname_np(this->m_native.Handle, name.c_str()); rc != 0)
            {
                AE_PANIC("pthread_setname_np (rc: {}, `{}`)", rc, strerror(rc));
            }
#endif
        }

        if (start.Priority)
        {
            sched_param sched{};
            int32_t policy = SCHED_RR;
#if true
            if (pthread_getschedparam(this->m_native.Handle, &policy, &sched) == 0)
            {
                sched.sched_priority = Private::ConvertThreadPriority(*start.Priority);

                pthread_setschedparam(this->m_native.Handle, policy, &sched);
            }

#else
            if (int const rc = pthread_getschedparam(this->m_native.Handle, &policy, &sched); rc != 0)
            {
                AE_PANIC("pthread_getschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }

            sched.sched_priority = Private::ConvertThreadPriority(*start.Priority);

            if (int const rc = pthread_setschedparam(this->m_native.Handle, policy, &sched); rc != 0)
            {
                AE_PANIC("pthread_setschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }
#endif
        }
    }

    Thread::Thread(Thread&& other) noexcept
        : m_native{std::exchange(other.m_native, {})}
    {
    }

    Thread& Thread::operator=(Thread&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->IsJoinable())
            {
                this->Join();
            }

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    Thread::~Thread()
    {
        if (this->IsJoinable())
        {
            this->Join();
        }
    }

    void Thread::Join()
    {
        if (this->m_native.Handle == 0)
        {
            AE_PANIC("Cannot join non-started thread.");
        }

        if (this->m_native.Handle == pthread_self())
        {
            AE_PANIC("Joining thread from itself");
        }

        if (int const rc = pthread_join(this->m_native.Handle, nullptr); rc != 0)
        {
            AE_PANIC("pthread_join (rc: {}, `{}`)", rc, strerror(rc));
        }

        this->m_native = {};
    }

    [[nodiscard]] bool Thread::IsJoinable() const
    {
        return this->m_native.Handle != 0;
    }

    [[nodiscard]] ThreadId Thread::GetId() const
    {
        return std::bit_cast<ThreadId>(this->m_native.Handle);
    }

    void Thread::Detach()
    {
        if (this->m_native.Handle == 0)
        {
            AE_PANIC("Failed to detach from thread");
        }

        if (int const rc = pthread_detach(this->m_native.Handle); rc != 0)
        {
            AE_PANIC("pthread_detach (rc: {}, `{}`)", rc, strerror(rc));
        }

        this->m_native = {};
    }

    ThreadId GetThisThreadId()
    {
        return std::bit_cast<ThreadId>(pthread_self());
    }
}
