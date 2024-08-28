#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <cstdint>
#include <atomic>

namespace Anemone::Threading
{
    class RUNTIME_API SemaphoreSlim final
    {
    private:
        std::atomic<int32_t> m_Count{};
        std::atomic<int32_t> m_Waiting{};

    public:
        explicit SemaphoreSlim(int32_t initial)
            : m_Count{initial}
        {
        }

        SemaphoreSlim(SemaphoreSlim const&) = delete;
        SemaphoreSlim(SemaphoreSlim&&) = delete;
        SemaphoreSlim& operator=(SemaphoreSlim const&) = delete;
        SemaphoreSlim& operator=(SemaphoreSlim&&) = delete;
        ~SemaphoreSlim();

        void Release(int32_t count = 1);

        void Acquire();

        bool TryAcquire();

    private:
        void Wait();
    };
}
