#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

#include <cmath>

namespace Anemone::Private
{
    namespace
    {
        constexpr int ConvertThreadPriority(ThreadPriority value)
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

        DWORD WINAPI ThreadEntryPoint(LPVOID lpThreadParameter)
        {
            if (lpThreadParameter == nullptr)
            {
                AE_PANIC("Thread started without context.");
            }

            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

            {
                Runnable* const runnable = static_cast<Runnable*>(lpThreadParameter);
                runnable->Run();
            }

            CoUninitialize();

            return 0;
        }
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
            AE_PANIC("Cannot create thread without runnable object");
        }

        size_t const stackSize = start.StackSize.value_or(0);

        // Create thread suspended so we can update internal structs before starting the thread.
        DWORD dwCreationFlags = CREATE_SUSPENDED;

        if (start.StackSize)
        {
            dwCreationFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
        }

        this->m_native.Handle = CreateThread(
            nullptr,
            stackSize,
            Private::ThreadEntryPoint,
            start.Callback,
            dwCreationFlags,
            &this->m_native.Id);

        if (this->m_native.Handle == nullptr)
        {
            AE_PANIC("Cannot create thread");
        }

        if (start.Name)
        {
            Interop::win32_string_buffer<wchar_t, 128> wname{};
            Interop::win32_WidenString(wname, *start.Name);

            SetThreadDescription(this->m_native.Handle, wname);
        }

        if (start.Priority)
        {
            SetThreadPriority(this->m_native.Handle, Private::ConvertThreadPriority(*start.Priority));
        }

        if (ResumeThread(this->m_native.Handle) == static_cast<DWORD>(-1))
        {
            AE_PANIC("Cannot resume thread");
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
        if (this->m_native.Handle == nullptr)
        {
            AE_PANIC("Failed to join thread");
        }

        if (this->m_native.Id == GetCurrentThreadId())
        {
            AE_PANIC("Joining thread from itself");
        }

        WaitForSingleObject(this->m_native.Handle, INFINITE);

        CloseHandle(this->m_native.Handle);

        this->m_native = {};
    }

    [[nodiscard]] bool Thread::IsJoinable() const
    {
        return this->m_native.Handle != nullptr;
    }

    [[nodiscard]] ThreadId Thread::GetId() const
    {
        return ThreadId{this->m_native.Id};
    }

    void Thread::Detach()
    {
        if (this->m_native.Handle == nullptr)
        {
            AE_PANIC("Failed to detach from thread");
        }

        CloseHandle(this->m_native.Handle);

        this->m_native = {};
    }

    ThreadId GetThisThreadId()
    {
        return ThreadId{GetCurrentThreadId()};
    }
}
