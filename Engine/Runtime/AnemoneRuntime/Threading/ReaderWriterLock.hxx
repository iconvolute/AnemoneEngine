#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/UninitializedStorage.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>

namespace Anemone::Platform
{
#if ANEMONE_PLATFORM_WINDOWS
    using NativeReaderWriterLockStorage = UninitializedStorage<struct NativeReaderWriterLock, 8, 8>;
#elif ANEMONE_PLATFORM_LINUX
    using NativeReaderWriterLockStorage = UninitializedStorage<struct NativeReaderWriterLock, 56, 8>;
#elif ANEMONE_PLATFORM_ANDROID
    using NativeReaderWriterLockStorage = UninitializedStorage<struct NativeReaderWriterLock, 56, 4>;
#else
#error "Not implemented"
#endif
}

namespace Anemone::Threading
{
    class RUNTIME_API ReaderWriterLock final
    {
    private:
        Platform::NativeReaderWriterLockStorage _native;

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
