#pragma once
#include <atomic>

namespace Anemone::Threading
{
    class CancellationToken final
    {
    private:
        std::atomic<bool> _cancelled{false};

    public:
        CancellationToken() = default;
        CancellationToken(CancellationToken const&) = delete;
        CancellationToken(CancellationToken&&) = delete;
        CancellationToken& operator=(CancellationToken const&) = delete;
        CancellationToken& operator=(CancellationToken&&) = delete;
        ~CancellationToken() = default;

    public:
        [[nodiscard]] bool IsCancelled() const
        {
            return this->_cancelled.load(std::memory_order_relaxed);
        }

        void Cancel()
        {
            this->_cancelled.store(true, std::memory_order_relaxed);
        }

        void Reset()
        {
            this->_cancelled.store(false, std::memory_order_relaxed);
        }
    };
}
