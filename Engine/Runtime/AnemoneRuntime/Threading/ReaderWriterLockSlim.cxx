#include "AnemoneRuntime/Threading/ReaderWriterLockSlim.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"

namespace Anemone::Threading
{
    void ReaderWriterLockSlim::EnterShared()
    {
        ThisThread::WaitForCompletion([this]
        {
            return this->TryEnterShared();
        });
    }

    bool ReaderWriterLockSlim::TryEnterShared()
    {
        int32_t value = this->m_bits.fetch_add(Reader, std::memory_order::acquire);

        if ((value & (Writer | Upgraded)))
        {
            this->m_bits.fetch_add(-Reader, std::memory_order::release);
            return false;
        }

        return true;
    }

    void ReaderWriterLockSlim::LeaveShared()
    {
        this->m_bits.fetch_add(-Reader, std::memory_order::release);
    }

    void ReaderWriterLockSlim::Enter()
    {
        ThisThread::WaitForCompletion([this]
        {
            return this->TryEnter();
        });
    }

    bool ReaderWriterLockSlim::TryEnter()
    {
        int32_t expected = 0;
        return this->m_bits.compare_exchange_strong(expected, Writer, std::memory_order::acq_rel);
    }

    void ReaderWriterLockSlim::Leave()
    {
        static_assert(Reader > (Writer + Upgraded));
        this->m_bits.fetch_and(~(Writer | Upgraded), std::memory_order::release);
    }

    void ReaderWriterLockSlim::LeaveAndEnterShared()
    {
        this->m_bits.fetch_add(Reader, std::memory_order_acquire);
        Leave();
    }

    void ReaderWriterLockSlim::EnterUpgrade()
    {
        ThisThread::WaitForCompletion([this]
        {
            return this->TryEnterUpgrade();
        });
    }

    void ReaderWriterLockSlim::LeaveUpgrade()
    {
        this->m_bits.fetch_add(-Upgraded, std::memory_order_acq_rel);
    }

    void ReaderWriterLockSlim::LeaveUpgradeAndEnter()
    {
        ThisThread::WaitForCompletion([this]
        {
            return this->TryLeaveUpgradeAndEnter();
        });
    }

    void ReaderWriterLockSlim::LeaveUpgradeAndEnterShared()
    {
        this->m_bits.fetch_add(Reader - Upgraded, std::memory_order_acq_rel);
    }

    void ReaderWriterLockSlim::LeaveAndEnterUpgrade()
    {
        this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);
        this->m_bits.fetch_add(-Writer, std::memory_order_release);
    }

    bool ReaderWriterLockSlim::TryLeaveUpgradeAndEnter()
    {
        int32_t expect = Upgraded;
        return this->m_bits.compare_exchange_strong(expect, Writer, std::memory_order_acq_rel);
    }

    bool ReaderWriterLockSlim::TryEnterUpgrade()
    {
        int32_t value = this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);

        return ((value & (Upgraded | Writer)) == 0);
    }
}
