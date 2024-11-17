#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#if !ANEMONE_FEATURE_FUTEX

#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    using UserCriticalSection = CriticalSection;
}

#else

#include <type_traits>
#include <atomic>
#include <cstdint>

namespace Anemone
{
    class RUNTIME_API UserCriticalSection final
    {
    private:
        std::atomic<int32_t> m_Flag{};

        static constexpr std::int32_t StateLocked = 1;
        static constexpr std::int32_t StateUnlocked = 0;

    public:
        UserCriticalSection() = default;
        UserCriticalSection(UserCriticalSection const&) = delete;
        UserCriticalSection(UserCriticalSection&&) = delete;
        UserCriticalSection& operator=(UserCriticalSection const&) = delete;
        UserCriticalSection& operator=(UserCriticalSection&&) = delete;
        ~UserCriticalSection() = default;

        void Enter();

        bool TryEnter();

        void Leave();

        template <typename F>
        auto With(F&& f) -> std::invoke_result_t<F&&>
        {
            this->Enter();
            auto result = std::forward<F>(f)();
            this->Leave();
            return result;
        }
    };
}

#endif
