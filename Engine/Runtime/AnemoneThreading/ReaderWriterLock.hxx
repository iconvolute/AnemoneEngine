#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneThreading/Lock.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneThreading/Platform/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneThreading/Platform/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

namespace Anemone
{
    class RUNTIME_API ReaderWriterLock final
    {
    private:
        Internal::PlatformReaderWriterLock _inner;

    public:
        ReaderWriterLock();
        ReaderWriterLock(ReaderWriterLock const&) = delete;
        ReaderWriterLock(ReaderWriterLock&&) = delete;
        ReaderWriterLock& operator=(ReaderWriterLock const&) = delete;
        ReaderWriterLock& operator=(ReaderWriterLock&&) = delete;
        ~ReaderWriterLock();

    public:
        void EnterShared();

        bool TryEnterShared();

        void LeaveShared();

        template <typename F>
        auto WithShared(F&& f) -> std::invoke_result_t<F&&>
        {
            SharedLock scope{*this};
            return std::forward<F>(f)();
        }

    public:
        void Enter();

        bool TryEnter();

        void Leave();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock scope{*this};
            return std::forward<F>(f)();
        }
    };
}
