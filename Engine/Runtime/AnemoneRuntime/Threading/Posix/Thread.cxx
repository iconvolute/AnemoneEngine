#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

#include <cmath>

namespace Anemone::Threading::Private
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

namespace Anemone::Threading
{
    Thread::Thread()
    {
        Platform::Create(this->_native);
    }

    Thread::Thread(ThreadStart const& start)
    {
        Platform::NativeThread& nativeThis = Platform::Create(this->_native);

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

        if (int const rc = pthread_create(&nativeThis.Inner, &attr, Private::ThreadEntryPoint, start.Callback); rc != 0)
        {
            AE_PANIC("pthread_create (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_attr_destroy(&attr); rc != 0)
        {
            AE_PANIC("pthread_attr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (nativeThis.Inner == 0)
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
            pthread_setname_np(nativeThis.Inner, name.c_str());
#else
            if (int const rc = pthread_setname_np(nativeThis.Inner, name.c_str()); rc != 0)
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
            if (pthread_getschedparam(nativeThis.Inner, &policy, &sched) == 0)
            {
                sched.sched_priority = Private::ConvertThreadPriority(*start.Priority);

                pthread_setschedparam(nativeThis.Inner, policy, &sched);
            }

#else
            if (int const rc = pthread_getschedparam(nativeThis.Inner, &policy, &sched); rc != 0)
            {
                AE_PANIC("pthread_getschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }

            sched.sched_priority = Private::ConvertThreadPriority(*start.Priority);

            if (int const rc = pthread_setschedparam(nativeThis.Inner, policy, &sched); rc != 0)
            {
                AE_PANIC("pthread_setschedparam (rc: {}, `{}`)", rc, strerror(rc));
            }
#endif
        }
    }

    Thread::Thread(Thread&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    Thread& Thread::operator=(Thread&& other) noexcept
    {
        if (this != &other)
        {
            if (this->IsJoinable())
            {
                this->Join();
            }

            Platform::NativeThread& nativeThis = Platform::Get(this->_native);
            Platform::NativeThread& nativeOther = Platform::Get(other._native);
            nativeThis = std::exchange(nativeOther, {});
        }

        return *this;
    }

    Thread::~Thread()
    {
        if (this->IsJoinable())
        {
            this->Join();
        }

        Platform::Destroy(this->_native);
    }

    void Thread::Join()
    {
        Platform::NativeThread& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Inner == 0)
        {
            AE_PANIC("Cannot join non-started thread.");
        }

        if (nativeThis.Inner == pthread_self())
        {
            AE_PANIC("Joining thread from itself");
        }

        if (int const rc = pthread_join(nativeThis.Inner, nullptr); rc != 0)
        {
            AE_PANIC("pthread_join (rc: {}, `{}`)", rc, strerror(rc));
        }

        nativeThis.Inner = 0;
    }

    [[nodiscard]] bool Thread::IsJoinable() const
    {
        Platform::NativeThread const& nativeThis = Platform::Get(this->_native);

        return nativeThis.Inner != 0;
    }

    [[nodiscard]] ThreadId Thread::GetId() const
    {
        Platform::NativeThread const& nativeThis = Platform::Get(this->_native);

        return std::bit_cast<ThreadId>(nativeThis.Inner);
    }

    void Thread::Detach()
    {
        Platform::NativeThread& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Inner == 0)
        {
            AE_PANIC("Failed to detach from thread");
        }

        if (int const rc = pthread_detach(nativeThis.Inner); rc != 0)
        {
            AE_PANIC("pthread_detach (rc: {}, `{}`)", rc, strerror(rc));
        }

        nativeThis.Inner = {};
    }

    ThreadId GetThisThreadId()
    {
        return std::bit_cast<ThreadId>(pthread_self());
    }
}
