#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsNative.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Posix/PosixNative.hxx"
#else
#error Not implemented
#endif

#include <type_traits>

namespace Anemone
{
    class RUNTIME_API ReaderWriterLock final
    {
    private:
        Detail::NativeReaderWriterLock m_native;

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
}
