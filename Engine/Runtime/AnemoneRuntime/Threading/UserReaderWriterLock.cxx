#include "AnemoneRuntime/Threading/UserReaderWriterLock.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"

namespace Anemone
{
    void UserReaderWriterLock::EnterShared()
    {
        WaitForCompletion([this]
        {
            return this->TryEnterShared();
        });
    }

    bool UserReaderWriterLock::TryEnterShared()
    {
        int32_t value = this->m_bits.fetch_add(Reader, std::memory_order::acquire);

        if ((value & (Writer | Upgraded)))
        {
            this->m_bits.fetch_add(-Reader, std::memory_order::release);
            return false;
        }

        return true;
    }

    void UserReaderWriterLock::LeaveShared()
    {
        this->m_bits.fetch_add(-Reader, std::memory_order::release);
    }

    void UserReaderWriterLock::Enter()
    {
        WaitForCompletion([this]
        {
            return this->TryEnter();
        });
    }

    bool UserReaderWriterLock::TryEnter()
    {
        int32_t expected = 0;
        return this->m_bits.compare_exchange_strong(expected, Writer, std::memory_order::acq_rel);
    }

    void UserReaderWriterLock::Leave()
    {
        static_assert(Reader > (Writer + Upgraded));
        this->m_bits.fetch_and(~(Writer | Upgraded), std::memory_order::release);
    }

    void UserReaderWriterLock::LeaveAndEnterShared()
    {
        this->m_bits.fetch_add(Reader, std::memory_order_acquire);
        Leave();
    }

    void UserReaderWriterLock::EnterUpgrade()
    {
        WaitForCompletion([this]
        {
            return this->TryEnterUpgrade();
        });
    }

    void UserReaderWriterLock::LeaveUpgrade()
    {
        this->m_bits.fetch_add(-Upgraded, std::memory_order_acq_rel);
    }

    void UserReaderWriterLock::LeaveUpgradeAndEnter()
    {
        WaitForCompletion([this]
        {
            return this->TryLeaveUpgradeAndEnter();
        });
    }

    void UserReaderWriterLock::LeaveUpgradeAndEnterShared()
    {
        this->m_bits.fetch_add(Reader - Upgraded, std::memory_order_acq_rel);
    }

    void UserReaderWriterLock::LeaveAndEnterUpgrade()
    {
        this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);
        this->m_bits.fetch_add(-Writer, std::memory_order_release);
    }

    bool UserReaderWriterLock::TryLeaveUpgradeAndEnter()
    {
        int32_t expect = Upgraded;
        return this->m_bits.compare_exchange_strong(expect, Writer, std::memory_order_acq_rel);
    }

    bool UserReaderWriterLock::TryEnterUpgrade()
    {
        int32_t value = this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);

        return ((value & (Upgraded | Writer)) == 0);
    }
}
