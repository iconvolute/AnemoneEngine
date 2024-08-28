#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <atomic>
#include <type_traits>

namespace Anemone::Threading
{
    class RUNTIME_API ReaderWriterLockSlim final
    {
    private:
        std::atomic<int32_t> m_bits{};

        static constexpr int32_t Reader = 4;
        static constexpr int32_t Upgraded = 2;
        static constexpr int32_t Writer = 1;

    public:
        ReaderWriterLockSlim() = default;
        ReaderWriterLockSlim(ReaderWriterLockSlim const&) = delete;
        ReaderWriterLockSlim(ReaderWriterLockSlim&&) = delete;
        ReaderWriterLockSlim& operator=(ReaderWriterLockSlim const&) = delete;
        ReaderWriterLockSlim& operator=(ReaderWriterLockSlim&&) = delete;
        ~ReaderWriterLockSlim() = default;

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
