#include "AnemoneRuntime/Threading/Windows/WindowsThread.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <cmath>

namespace Anemone::Private
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

    static DWORD WINAPI ThreadEntryPoint(LPVOID lpThreadParameter)
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

namespace Anemone
{
    WindowsThread::WindowsThread()
    {
        this->m_handle = nullptr;
        this->m_id = 0;
    }

    WindowsThread::WindowsThread(ThreadStart const& start)
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

        this->m_handle = CreateThread(
            nullptr,
            stackSize,
            Private::ThreadEntryPoint,
            start.Callback,
            dwCreationFlags,
            &this->m_id);

        if (this->m_handle == nullptr)
        {
            AE_PANIC("Cannot create thread");
        }

        if (start.Name)
        {
            Platform::win32_string_buffer<wchar_t, 128> wname{};
            Platform::win32_WidenString(wname, *start.Name);

            SetThreadDescription(this->m_handle, wname);
        }

        if (start.Priority)
        {
            SetThreadPriority(this->m_handle, Private::ConvertThreadPriority(*start.Priority));
        }

        if (ResumeThread(this->m_handle) == static_cast<DWORD>(-1))
        {
            AE_PANIC("Cannot resume thread");
        }
    }

    WindowsThread::WindowsThread(WindowsThread&& other) noexcept
        : m_handle{std::exchange(other.m_handle, {})}
        , m_id{std::exchange(other.m_id, {})}
    {
    }

    WindowsThread& WindowsThread::operator=(WindowsThread&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->IsJoinable())
            {
                this->Join();
            }

            this->m_handle = std::exchange(other.m_handle, {});
            this->m_id = std::exchange(other.m_id, {});
        }

        return *this;
    }

    WindowsThread::~WindowsThread()
    {
        if (this->IsJoinable())
        {
            this->Join();
        }
    }

    void WindowsThread::Join()
    {
        if (this->m_handle == nullptr)
        {
            AE_PANIC("Failed to join thread");
        }

        if (this->m_id == GetCurrentThreadId())
        {
            AE_PANIC("Joining thread from itself");
        }

        WaitForSingleObject(this->m_handle, INFINITE);

        CloseHandle(this->m_handle);

        this->m_handle = nullptr;
        this->m_id = 0;
    }

    [[nodiscard]] bool WindowsThread::IsJoinable() const
    {
        return this->m_handle != nullptr;
    }

    [[nodiscard]] ThreadId WindowsThread::GetId() const
    {
        return ThreadId{this->m_id};
    }

    void WindowsThread::Detach()
    {
        if (this->m_handle == nullptr)
        {
            AE_PANIC("Failed to detach from thread");
        }

        CloseHandle(this->m_handle);

        this->m_handle = nullptr;
        this->m_id = 0;
    }

    ThreadId GetThisThreadId()
    {
        return ThreadId{GetCurrentThreadId()};
    }
}
