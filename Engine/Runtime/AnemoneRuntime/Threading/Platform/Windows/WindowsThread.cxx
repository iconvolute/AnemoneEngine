#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/Platform/Windows/WindowsThread.hxx"
#include "AnemoneRuntime/Base/Instant.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Threading/SpinWait.hxx"

namespace Anemone
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
    }

    DWORD WINAPI WindowsThread::EntryPoint(LPVOID lpThreadParameter)
    {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        {
            AE_ASSERT(lpThreadParameter);

            StartupContext& context = *static_cast<StartupContext*>(lpThreadParameter);

            WindowsThread* self = context.thread.load(std::memory_order::acquire);
            AE_ASSERT(self);

            context.initialized.store(true, std::memory_order::release);

            self->_runnable->Run();
            self->ReleaseReference();
        }
        CoUninitialize();

        return 0;
    }

    WindowsThread::~WindowsThread()
    {
        if (this->_handle)
        {
            AE_PANIC("Destroying joinable thread. Did you forget to call Join or Detach?");
        }
    }

    void WindowsThread::Join()
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(this->_id != ThreadId{GetCurrentThreadId()});

        if (WaitForSingleObject(this->_handle.Get(), INFINITE) != WAIT_OBJECT_0)
        {
            AE_PANIC("Failed to join thread");
        }

        this->_handle = {};
        this->_id = {};
    }

    bool WindowsThread::IsJoinable() const
    {
        return this->_handle.IsValid();
    }

    void WindowsThread::Detach()
    {
        AE_ASSERT(this->_handle);
        AE_ASSERT(this->_id != ThreadId{GetCurrentThreadId()});

        if (this->_handle)
        {
            this->_handle = {};
            this->_id = {};
        }
    }

    Reference<WindowsThread> WindowsThread::Start(ThreadStart const& start)
    {
        if (not start.Callback)
        {
            AE_PANIC("Cannot create thread without runnable object");
        }

        Reference<WindowsThread> result = MakeReference<WindowsThread>();

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

            // CreateThread parameters.
            DWORD dwCreationFlags = CREATE_SUSPENDED;

            size_t stackSize = start.StackSize.value_or(0);

            if (start.StackSize)
            {
                dwCreationFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
            }

            DWORD dwThreadId{};
            result->_handle.Attach(
                CreateThread(
                    nullptr,
                    stackSize,
                    &EntryPoint,
                    &context,
                    dwCreationFlags,
                    &dwThreadId));

            result->_id = ThreadId{dwThreadId};

            AE_ASSERT(result->_handle, "Failed to spawn thread");

            if (start.Priority)
            {
                SetThreadPriority(
                    result->_handle.Get(),
                    ConvertThreadPriority(*start.Priority));
            }


            if (start.Name)
            {
                Interop::string_buffer<wchar_t, 128> wName{};
                Interop::Windows::WidenString(wName, *start.Name);

                SetThreadDescription(result->_handle.Get(), wName);
            }

            ResumeThread(result->_handle.Get());

            WaitForCompletion(context.initialized);
        }

        return result;
    }
}

namespace Anemone
{
    Reference<Thread> Thread::Start(ThreadStart const& threadStart)
    {
        return WindowsThread::Start(threadStart);
    }
}
