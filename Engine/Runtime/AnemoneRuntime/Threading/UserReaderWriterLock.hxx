#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#if !ANEMONE_FEATURE_FUTEX

#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"

namespace Anemone
{
    using UserReaderWriterLock = ReaderWriterLock;
}

#else

#include <atomic>
#include <type_traits>

namespace Anemone
{
    class RUNTIME_API UserReaderWriterLock final
    {
    private:
        std::atomic<int32_t> m_bits{};

        static constexpr int32_t Reader = 4;
        static constexpr int32_t Upgraded = 2;
        static constexpr int32_t Writer = 1;

    public:
        UserReaderWriterLock() = default;
        UserReaderWriterLock(UserReaderWriterLock const&) = delete;
        UserReaderWriterLock(UserReaderWriterLock&&) = delete;
        UserReaderWriterLock& operator=(UserReaderWriterLock const&) = delete;
        UserReaderWriterLock& operator=(UserReaderWriterLock&&) = delete;
        ~UserReaderWriterLock() = default;

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

    public:
        void LeaveAndEnterShared();
        void EnterUpgrade();
        void LeaveUpgrade();
        void LeaveUpgradeAndEnter();
        void LeaveUpgradeAndEnterShared();
        void LeaveAndEnterUpgrade();
        bool TryLeaveUpgradeAndEnter();
        bool TryEnterUpgrade();
    };
}

#endif
