#pragma once
#include <utility>
#include <functional>

// CXX-Next: Replace this with proper std::function_ref when it's available.
namespace Anemone
{
    template <typename T>
    class FunctionRef;

    template <typename R, typename... ArgsT>
    class FunctionRef<R(ArgsT...)>
    {
    private:
        void* m_Instance{};
        R (*m_Function)(void*, ArgsT...) = nullptr;

    public:
        constexpr FunctionRef() noexcept = default;

        constexpr FunctionRef(FunctionRef const&) noexcept = default;

        template <typename F>
            requires(!std::is_same_v<std::decay_t<F>, FunctionRef> && std::is_invocable_r_v<R, F &&, ArgsT...>)
        constexpr FunctionRef(F&& f) noexcept // NOLINT(bugprone-forwarding-reference-overload)
            : m_Instance{const_cast<void*>(static_cast<void const*>(std::addressof(f)))}
        {
            this->m_Function = [](void* instance, ArgsT... args) -> R
            {
                return std::invoke(*reinterpret_cast<std::add_pointer_t<F>>(instance), std::forward<ArgsT>(args)...);
            };
        }

        constexpr FunctionRef& operator=(FunctionRef const&) noexcept = default;

        template <typename F>
            requires(!std::is_same_v<std::decay_t<F>, FunctionRef> && std::is_invocable_r_v<R, F &&, ArgsT...>)
        constexpr FunctionRef& operator=(F&& f) noexcept
        {
            this->m_Instance = reinterpret_cast<void*>(std::addressof(f));
            this->m_Function = [](void* instance, ArgsT... args) -> R
            {
                return std::invoke(*reinterpret_cast<std::add_pointer_t<F>>(instance), std::forward<ArgsT>(args)...);
            };

            return *this;
        }

        constexpr void swap(FunctionRef& other) noexcept
        {
            std::swap(this->m_Instance, other.m_Instance);
            std::swap(this->m_Function, other.m_Function);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->m_Function != nullptr;
        }

        constexpr R operator()(ArgsT... args) const noexcept
        {
            return this->m_Function(this->m_Instance, std::forward<ArgsT>(args)...);
        }
    };


    template <typename R, typename... ArgsT>
    FunctionRef(R (*)(ArgsT...)) -> FunctionRef<R(ArgsT...)>;
}
