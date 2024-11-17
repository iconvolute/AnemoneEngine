#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>

namespace Anemone
{
    class RUNTIME_API WindowsReaderWriterLock final
    {
    private:
        SRWLOCK m_native;

    public:
        WindowsReaderWriterLock();
        WindowsReaderWriterLock(WindowsReaderWriterLock const&) = delete;
        WindowsReaderWriterLock(WindowsReaderWriterLock&&) = delete;
        WindowsReaderWriterLock& operator=(WindowsReaderWriterLock const&) = delete;
        WindowsReaderWriterLock& operator=(WindowsReaderWriterLock&&) = delete;
        ~WindowsReaderWriterLock();

    public:
        void EnterShared();
        bool TryEnterShared();
        void LeaveShared();

        template <typename F>
        auto WithShared(F&& f) -> std::invoke_result_t<F&&>
        {
            SharedLock lock{*this};
            return std::forward<F>(f)();
        }

    public:
        void Enter();
        bool TryEnter();
        void Leave();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock lock{*this};
            return std::forward<F>(f)();
        }
    };

    using ReaderWriterLock = WindowsReaderWriterLock;
}
