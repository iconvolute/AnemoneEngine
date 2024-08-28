#pragma once

namespace Anemone::Threading
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

namespace Anemone::Threading
{
    template <typename LockT>
        requires(UniqueLockable<LockT>)
    class [[nodiscard]] UniqueLock final
    {
    private:
        LockT& _lock;

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
    };

    template <typename LockT>
        requires(SharedLockable<LockT>)
    class [[nodiscard]] SharedLock final
    {
    private:
        LockT& _lock;

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
    };
}
