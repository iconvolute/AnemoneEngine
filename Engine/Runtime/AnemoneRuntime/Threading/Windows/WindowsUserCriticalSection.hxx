#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>
#include <atomic>
#include <cstdint>

namespace Anemone
{
    class RUNTIME_API WindowsUserCriticalSection final
    {
    private:
        std::atomic<int32_t> m_Flag{};

        static constexpr std::int32_t StateLocked = 1;
        static constexpr std::int32_t StateUnlocked = 0;

    public:
        WindowsUserCriticalSection() = default;
        WindowsUserCriticalSection(WindowsUserCriticalSection const&) = delete;
        WindowsUserCriticalSection(WindowsUserCriticalSection&&) = delete;
        WindowsUserCriticalSection& operator=(WindowsUserCriticalSection const&) = delete;
        WindowsUserCriticalSection& operator=(WindowsUserCriticalSection&&) = delete;
        ~WindowsUserCriticalSection() = default;

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

    using UserCriticalSection = WindowsUserCriticalSection;
}
