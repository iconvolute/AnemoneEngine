#pragma once
#include <type_traits>
#include <utility>

namespace Anemone
{
    template <typename LockT>
    concept UniqueLockable = requires(LockT lock) {
        lock.Enter();
        lock.Leave();
    };

    template <typename LockT>
    concept SharedLockable = requires(LockT lock) {
        lock.EnterShared();
        lock.LeaveShared();
    };
}

namespace Anemone
{
    template <typename LockT>
        requires(UniqueLockable<LockT>)
    class [[nodiscard, jetbrains::guard]] UniqueLock final
    {
    private:
        LockT& _lock;

    public:
        using Lock = LockT;

    public:
        UniqueLock() = delete;
        UniqueLock(UniqueLock const&) = delete;
        UniqueLock(UniqueLock&&) = delete;
        UniqueLock& operator=(UniqueLock const&) = delete;
        UniqueLock& operator=(UniqueLock&&) = delete;

        explicit UniqueLock(LockT& lock)
            : _lock{lock}
        {
            this->_lock.Enter();
        }

        ~UniqueLock()
        {
            this->_lock.Leave();
        }

        Lock& GetLock() const
        {
            return this->_lock;
        }
    };

    template <typename LockT>
        requires(SharedLockable<LockT>)
    class [[nodiscard, jetbrains::guard]] SharedLock final
    {
    private:
        LockT& _lock;

    public:
        using Lock = LockT;

    public:
        SharedLock() = delete;
        SharedLock(SharedLock const&) = delete;
        SharedLock(SharedLock&&) = delete;
        SharedLock& operator=(SharedLock const&) = delete;
        SharedLock& operator=(SharedLock&&) = delete;

        explicit SharedLock(LockT& lock)
            : _lock{lock}
        {
            this->_lock.EnterShared();
        }

        ~SharedLock()
        {
            this->_lock.LeaveShared();
        }

        Lock& GetLock() const
        {
            return this->_lock;
        }
    };

    template <typename Lock, typename Callback>
    auto WithLock(Lock& lock, Callback&& callback) -> std::invoke_result_t<Callback&&>
    {
        UniqueLock _{lock};
        return std::forward<Callback>(callback)();
    }

    template <typename Lock, typename Callback>
    auto WithSharedLock(Lock& lock, Callback&& callback) -> std::invoke_result_t<Callback&&>
    {
        SharedLock _{lock};
        return std::forward<Callback>(callback)();
    }
}
