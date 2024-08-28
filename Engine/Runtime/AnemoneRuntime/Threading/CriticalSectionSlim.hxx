#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <type_traits>
#include <atomic>
#include <cstdint>

namespace Anemone::Threading
{
    class RUNTIME_API CriticalSectionSlim final
    {
    private:
        std::atomic<int32_t> m_Flag{};

        static constexpr std::int32_t StateLocked = 1;
        static constexpr std::int32_t StateUnlocked = 0;

    public:
        CriticalSectionSlim() = default;
        CriticalSectionSlim(CriticalSectionSlim const&) = delete;
        CriticalSectionSlim(CriticalSectionSlim&&) = delete;
        CriticalSectionSlim& operator=(CriticalSectionSlim const&) = delete;
        CriticalSectionSlim& operator=(CriticalSectionSlim&&) = delete;
        ~CriticalSectionSlim() = default;

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
