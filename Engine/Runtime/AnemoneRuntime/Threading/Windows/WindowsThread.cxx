#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/Windows/WindowsThread.hxx"
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

        this->_handle.Attach(CreateThread(
            nullptr,
            stackSize,
            Private::ThreadEntryPoint,
            start.Callback,
            dwCreationFlags,
            &this->_id));

        if (not this->_handle)
        {
            AE_PANIC("Cannot create thread");
        }

        if (start.Name)
        {
            Interop::string_buffer<wchar_t, 128> wname{};
            Interop::win32_WidenString(wname, *start.Name);

            SetThreadDescription(this->_handle.Get(), wname);
        }

        if (start.Priority)
        {
            SetThreadPriority(this->_handle.Get(), Private::ConvertThreadPriority(*start.Priority));
        }

        if (ResumeThread(this->_handle.Get()) == static_cast<DWORD>(-1))
        {
            AE_PANIC("Cannot resume thread");
        }
    }

    WindowsThread::WindowsThread(WindowsThread&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
        , _id{std::exchange(other._id, {})}
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

            this->_handle = std::exchange(other._handle, {});
            this->_id = std::exchange(other._id, {});
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
        if (not this->_handle)
        {
            AE_PANIC("Failed to join thread");
        }

        if (this->_id == GetCurrentThreadId())
        {
            AE_PANIC("Joining thread from itself");
        }

        WaitForSingleObject(this->_handle.Get(), INFINITE);

        this->_handle = {};
    }

    [[nodiscard]] bool WindowsThread::IsJoinable() const
    {
        return this->_handle.IsValid();
    }

    void WindowsThread::Detach()
    {
        if (not this->_handle)
        {
            AE_PANIC("Thread already detached");
        }

        this->_handle = {};
        this->_id = {};
    }
}
