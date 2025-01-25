#pragma once
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <atomic>
#include <type_traits>

namespace Anemone
{
    class ConcurrentAccess final
    {
    private:
#if ANEMONE_FEATURE_ASSERTIONS
        std::atomic_intptr_t m_Access{0};
#endif

    public:
        anemone_forceinline void EnterShared()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            AE_ASSERT(++this->m_Access > 0, "Concurrent read access detected while holding a write lock");
#endif
        }

        anemone_forceinline bool TryEnterShared()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            this->EnterShared();
#endif
            return true;
        }

        anemone_forceinline void LeaveShared()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            AE_ASSERT(--this->m_Access >= 0, "Mismatch between read/write access.");
#endif
        }

        anemone_forceinline void Enter()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            AE_ASSERT(--this->m_Access == -1, "Concurrent write access detected while holding a read lock.");
#endif
        }

        anemone_forceinline bool TryEnter()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            this->Enter();
#endif
            return true;
        }

        anemone_forceinline void Leave()
        {
#if ANEMONE_FEATURE_ASSERTIONS
            AE_ASSERT(++this->m_Access == 0, "Concurrent write access detected. Multiple threads are writing to the same resource.");
#endif
        }

        template <typename F>
        auto WithShared(F&& f) -> std::invoke_result_t<F&&>
        {
            SharedLock lock{*this};
            return std::forward<F>(f)();
        }

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            UniqueLock lock{*this};
            return std::forward<F>(f)();
        }
    };
}
