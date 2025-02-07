#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"
#include "AnemoneRuntime/Threading/SpinWait.hxx"

#include <atomic>

namespace Anemone
{
    template <typename FutexTraits>
    class BaseUserReaderWriterLock final
    {
    private:
        std::atomic<int32_t> m_bits{};

        static constexpr int32_t Reader = 4;
        static constexpr int32_t Upgraded = 2;
        static constexpr int32_t Writer = 1;

    public:
        BaseUserReaderWriterLock() = default;

        BaseUserReaderWriterLock(BaseUserReaderWriterLock const&) = delete;

        BaseUserReaderWriterLock(BaseUserReaderWriterLock&&) = delete;

        BaseUserReaderWriterLock& operator=(BaseUserReaderWriterLock const&) = delete;

        BaseUserReaderWriterLock& operator=(BaseUserReaderWriterLock&&) = delete;

        ~BaseUserReaderWriterLock() = default;

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
