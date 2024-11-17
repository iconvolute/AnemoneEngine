#pragma once
#include "AnemoneRuntime/Threading/Lock.hxx"

#include <cstddef>

namespace Anemone
{
    template <typename LockType, size_t N>
    class CriticalSectionPool final
    {
    private:
        LockType _sections[N]{};

    public:
        static constexpr size_t Count = N;

        CriticalSectionPool() = default;
        CriticalSectionPool(CriticalSectionPool const&) = delete;
        CriticalSectionPool(CriticalSectionPool&&) = delete;
        CriticalSectionPool& operator=(CriticalSectionPool const&) = delete;
        CriticalSectionPool& operator=(CriticalSectionPool&&) = delete;
        ~CriticalSectionPool() = default;

        UniqueLock<LockType> Lock(size_t key)
        {
            size_t const index = key % N;
            return UniqueLock<LockType>{this->_sections[index]};
        }

        template <typename F>
        auto With(size_t key, F&& f) -> std::invoke_result_t<F&&>
        {
            size_t const index = key % N;
            return this->_sections[index].With(std::forward<F>(f));
        }
    };
}
