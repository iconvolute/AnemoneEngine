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
            SpinWait spinner;
            while (true)
            {
                if (TryEnterShared())
                {
                    return;
                }
                
                int32_t current = m_bits.load(std::memory_order::acquire);
                if ((current & (Writer | Upgraded)))
                {
                    if (spinner.NextSpinWillYield())
                    {
                        Internal::Futex::Wait(m_bits, current);
                    }
                    else
                    {
                        spinner.SpinOnce();
                    }
                }
            }
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
            int32_t oldValue = this->m_bits.fetch_add(-Reader, std::memory_order::release);
            
            // If this was the last reader and someone might be waiting, wake them
            if (oldValue == Reader)
            {
                Internal::Futex::WakeOne(m_bits);
            }
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
            SpinWait spinner;
            while (true)
            {
                if (TryEnter())
                {
                    return;
                }
                
                int32_t current = m_bits.load(std::memory_order::acquire);
                if (current != 0)
                {
                    if (spinner.NextSpinWillYield())
                    {
                        Internal::Futex::Wait(m_bits, current);
                    }
                    else
                    {
                        spinner.SpinOnce();
                    }
                }
            }
        }

        bool TryEnter()
        {
            int32_t expected = 0;
            return this->m_bits.compare_exchange_strong(expected, Writer, std::memory_order::acq_rel);
        }

        void Leave()
        {
            static_assert(Reader > (Writer + Upgraded));
            int32_t oldValue = m_bits.fetch_and(~(Writer | Upgraded), std::memory_order::release);
            
            // If we released a writer lock and there might be waiters, wake them all
            // We wake all since we don't know if waiters are readers or writers
            if (oldValue & Writer)
            {
                Internal::Futex::WakeAll(m_bits);
            }
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
            SpinWait spinner;
            while (true)
            {
                if (TryEnterUpgrade())
                {
                    return;
                }
                
                int32_t current = m_bits.load(std::memory_order::acquire);
                if ((current & (Writer | Upgraded)))
                {
                    if (spinner.NextSpinWillYield())
                    {
                        Internal::Futex::Wait(m_bits, current);
                    }
                    else
                    {
                        spinner.SpinOnce();
                    }
                }
            }
        }

        void LeaveUpgrade()
        {
            int32_t oldValue = this->m_bits.fetch_add(-Upgraded, std::memory_order_acq_rel);
            
            // If anyone might be waiting, wake them
            if (oldValue & Upgraded)
            {
                Internal::Futex::WakeOne(m_bits);
            }
        }

        void LeaveUpgradeAndEnter()
        {
            SpinWait spinner;
            while (true)
            {
                if (TryLeaveUpgradeAndEnter())
                {
                    return;
                }
                
                int32_t current = m_bits.load(std::memory_order::acquire);
                if (current != Upgraded)
                {
                    if (spinner.NextSpinWillYield())
                    {
                        Internal::Futex::Wait(m_bits, current);
                    }
                    else
                    {
                        spinner.SpinOnce();
                    }
                }
            }
        }

        void LeaveUpgradeAndEnterShared()
        {
            this->m_bits.fetch_add(Reader - Upgraded, std::memory_order_acq_rel);
            // No need to wake as we're transitioning to a less exclusive state
        }

        void LeaveAndEnterUpgrade()
        {
            this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);
            this->m_bits.fetch_add(-Writer, std::memory_order_release);
            
            // Wake all readers since we're transitioning to a less exclusive state
            Internal::Futex::WakeAll(m_bits);
        }

        bool TryLeaveUpgradeAndEnter()
        {
            int32_t expect = Upgraded;
            return this->m_bits.compare_exchange_strong(expect, Writer, std::memory_order_acq_rel);
        }

        bool TryEnterUpgrade()
        {
            int32_t value = this->m_bits.fetch_or(Upgraded, std::memory_order_acquire);

            // If already locked for writer or upgrade, revert our change
            if ((value & (Upgraded | Writer)))
            {
                // Only revert if we actually made a change (i.e., Upgraded wasn't already set)
                if (!(value & Upgraded))
                {
                    this->m_bits.fetch_and(~Upgraded, std::memory_order_release);
                }
                return false;
            }

            return true;
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
