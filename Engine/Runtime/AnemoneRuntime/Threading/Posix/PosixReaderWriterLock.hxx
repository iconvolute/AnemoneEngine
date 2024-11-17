#pragma once
#include "AnemoneRuntime/Platform/Posix/Headers.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>

namespace Anemone
{
    class RUNTIME_API PosixReaderWriterLock final
    {
    private:
        pthread_rwlock_t m_native;

    public:
        PosixReaderWriterLock();
        PosixReaderWriterLock(PosixReaderWriterLock const&) = delete;
        PosixReaderWriterLock(PosixReaderWriterLock&&) = delete;
        PosixReaderWriterLock& operator=(PosixReaderWriterLock const&) = delete;
        PosixReaderWriterLock& operator=(PosixReaderWriterLock&&) = delete;
        ~PosixReaderWriterLock();

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

    using ReaderWriterLock = PosixReaderWriterLock;
}
