#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_FEATURE_FUTEX

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Threading/Windows/WindowsThreading.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Threading/Unix/UnixThreading.hxx"
#else
#error Not implemented
#endif

#include "AnemoneRuntime/Threading/Lock.hxx"
#include "AnemoneRuntime/Threading/SpinWait.hxx"

#include <atomic>

namespace Anemone
{
    // NOTE: This implementation does not use futexes.
    class UserReaderWriterLock final
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
        void EnterShared()
        {
            WaitForCompletion([this]
            {
                return this->TryEnterShared();
            });
        }

        bool TryEnterShared()
        {
            int32_t value = this->m_bits.fetch_add(Reader, std::memory_order::acquire);

            if ((value & (Writer | Upgraded)))
            {
                this->m_bits.fetch_add(-Reader, std::memory_order::release);
                return false;
            }

            return true;
        }

        void LeaveShared()
        {
            this->m_bits.fetch_add(-Reader, std::memory_order::release);
        }

        template <typename F>
        auto WithShared(F&& f) -> std::invoke_result_t<F&&>
        {
            SharedLock scope{*this};
            return std::forward<F>(f)();
        }

    public:
        void Enter()
        {
            WaitForCompletion([this]
            {
                return this->TryEnter();
            });
        }

        bool TryEnter()
        {
            int32_t expected = 0;
            return this->m_bits.compare_exchange_strong(expected, Writer, std::memory_order::acq_rel);
        }

        void Leave()
        {
            static_assert(Reader > (Writer + Upgraded));
            this->m_bits.fetch_and(~(Writer | Upgraded), std::memory_order::release);
        }

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock scope{*this};
            return std::forward<F>(f)();
        }

    public:
        void LeaveAndEnterShared()
        {
            this->m_bits.fetch_add(Reader, std::memory_order_acquire);
            Leave();
        }

        void EnterUpgrade()
        {
            WaitForCompletion([this]
            {
                return this->TryEnterUpgrade();
            });
        }

        void LeaveUpgrade()
        {
            this->m_bits.fetch_add(-Upgraded, std::memory_order_acq_rel);
        }

        void LeaveUpgradeAndEnter()
        {
            WaitForCompletion([this]
            {
                return this->TryLeaveUpgradeAndEnter();
            });
        }

        void LeaveUpgradeAndEnterShared()
        {
            this->m_bits.fetch_add(Reader - Upgraded, std::memory_order_acq_rel);
        }

        void LeaveAndEnterUpgrade()
        {
            this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);
            this->m_bits.fetch_add(-Writer, std::memory_order_release);
        }

        bool TryLeaveUpgradeAndEnter()
        {
            int32_t expect = Upgraded;
            return this->m_bits.compare_exchange_strong(expect, Writer, std::memory_order_acq_rel);
        }

        bool TryEnterUpgrade()
        {
            int32_t value = this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);

            return ((value & (Upgraded | Writer)) == 0);
        }
    };
}

#else

#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"

namespace Anemone
{
    using UserReaderWriterLock = ReaderWriterLock;
}

#endif
