#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <cmath>

namespace Anemone::Threading::Private
{
    constexpr int ConvertThreadPriority(ThreadPriority value) noexcept
    {
        switch (value)
        {
        case ThreadPriority::TimeCritical:
            return THREAD_PRIORITY_TIME_CRITICAL;
        case ThreadPriority::Highest:
            return THREAD_PRIORITY_HIGHEST;
        case ThreadPriority::AboveNormal:
            return THREAD_PRIORITY_ABOVE_NORMAL;
        case ThreadPriority::Normal:
            return THREAD_PRIORITY_NORMAL;
        case ThreadPriority::BelowNormal:
            return THREAD_PRIORITY_BELOW_NORMAL;
        case ThreadPriority::Lower:
            return THREAD_PRIORITY_NORMAL - 1;
        case ThreadPriority::Lowest:
            return THREAD_PRIORITY_LOWEST;
        }

        return THREAD_PRIORITY_NORMAL;
    }

    static DWORD WINAPI ThreadEntryPoint(LPVOID lpThreadParameter)
    {
        if (lpThreadParameter == nullptr)
        {
            AE_PANIC("Thread started without context.");
        }

        Runnable* const runnable = static_cast<Runnable*>(lpThreadParameter);
        runnable->Run();

        return 0;
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
            AE_PANIC("Cannot create thread without runnable object");
        }

        size_t const stackSize = start.StackSize.value_or(0);

        // Create thread suspended so we can update internal structs before starting the thread.
        DWORD dwCreationFlags = CREATE_SUSPENDED;

        if (start.StackSize)
        {
            dwCreationFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
        }

        nativeThis.Handle = CreateThread(
            nullptr,
            stackSize,
            Private::ThreadEntryPoint,
            start.Callback,
            dwCreationFlags,
            &nativeThis.Id);

        if (nativeThis.Handle == nullptr)
        {
            AE_PANIC("Cannot create thread");
        }

        if (start.Name)
        {
            Platform::win32_string_buffer<wchar_t, 128> wname{};
            Platform::win32_WidenString(wname, *start.Name);

            SetThreadDescription(nativeThis.Handle, wname);
        }

        if (start.Priority)
        {
            SetThreadPriority(nativeThis.Handle, Private::ConvertThreadPriority(*start.Priority));
        }

        if (ResumeThread(nativeThis.Handle) == static_cast<DWORD>(-1))
        {
            AE_PANIC("Cannot resume thread");
        }
    }

    Thread::Thread(Thread&& other) noexcept
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    Thread& Thread::operator=(Thread&& other) noexcept
    {
        if (this != std::addressof(other))
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

        if (nativeThis.Handle == nullptr)
        {
            AE_PANIC("Failed to join thread");
        }

        if (nativeThis.Id == GetCurrentThreadId())
        {
            AE_PANIC("Joining thread from itself");
        }

        WaitForSingleObject(nativeThis.Handle, INFINITE);

        CloseHandle(nativeThis.Handle);

        nativeThis = {};
    }

    [[nodiscard]] bool Thread::IsJoinable() const
    {
        Platform::NativeThread const& nativeThis = Platform::Get(this->_native);

        return nativeThis.Handle != nullptr;
    }

    [[nodiscard]] ThreadId Thread::GetId() const
    {
        Platform::NativeThread const& nativeThis = Platform::Get(this->_native);

        return ThreadId{nativeThis.Id};
    }

    void Thread::Detach()
    {
        Platform::NativeThread& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle == nullptr)
        {
            AE_PANIC("Failed to detach from thread");
        }

        CloseHandle(nativeThis.Handle);

        nativeThis = {};
    }

    ThreadId GetThisThreadId()
    {
        return ThreadId{GetCurrentThreadId()};
    }
}
