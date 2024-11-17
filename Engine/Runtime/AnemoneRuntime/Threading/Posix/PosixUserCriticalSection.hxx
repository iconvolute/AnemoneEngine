#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>
#include <atomic>
#include <cstdint>

namespace Anemone
{
    class RUNTIME_API PosixUserCriticalSection final
    {
    private:
        std::atomic<int32_t> m_Flag{};

        static constexpr std::int32_t StateLocked = 1;
        static constexpr std::int32_t StateUnlocked = 0;

    public:
        PosixUserCriticalSection() = default;
        PosixUserCriticalSection(PosixUserCriticalSection const&) = delete;
        PosixUserCriticalSection(PosixUserCriticalSection&&) = delete;
        PosixUserCriticalSection& operator=(PosixUserCriticalSection const&) = delete;
        PosixUserCriticalSection& operator=(PosixUserCriticalSection&&) = delete;
        ~PosixUserCriticalSection() = default;

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

    using UserCriticalSection = PosixUserCriticalSection;
}
