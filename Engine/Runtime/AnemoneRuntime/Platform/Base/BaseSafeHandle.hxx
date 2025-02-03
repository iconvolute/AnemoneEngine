#pragma once
#include <type_traits>
#include <utility>

namespace Anemone::Interop
{
    template <typename T, typename TraitsT>
    class base_SafeHandle final
    {
    private:
        T _value = TraitsT::Invalid();

    public:
        base_SafeHandle() = default;

        explicit base_SafeHandle(T value)
            : _value{value}
        {
        }

        base_SafeHandle(base_SafeHandle const&) = delete;

        base_SafeHandle(base_SafeHandle&& other) noexcept
            : _value{std::exchange(other._value, TraitsT::Invalid())}
        {
        }

        base_SafeHandle& operator=(base_SafeHandle const&) = delete;

        base_SafeHandle& operator=(base_SafeHandle&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->Attach(other.Detach());
            }

            return *this;
        }

        ~base_SafeHandle()
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Close(this->_value);
            }
        }

    public:
        [[nodiscard]] constexpr bool IsValid() const
        {
            return TraitsT::IsValid(this->_value);
        }

        [[nodiscard]] explicit constexpr operator bool() const
        {
            return this->IsValid();
        }

        [[nodiscard]] constexpr T Value() const
        {
            return this->_value;
        }

    public:
        T Detach()
        {
            return std::exchange(this->_value, TraitsT::Invalid());
        }

        void Attach(T value)
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Close(this->_value);
            }

            this->_value = value;
        }

        void Close()
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Close(this->_value);
                this->_value = TraitsT::Invalid();
            }
        }
    };
}
