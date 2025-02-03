#pragma once

namespace Anemone::Interop
{
    template <typename HandleT, typename TraitsT>
    class base_SafeHandle final
    {
    private:
        HandleT _value = TraitsT::Invalid();

    public:
        base_SafeHandle() = default;

        explicit base_SafeHandle(HandleT value)
            : _value{value}
        {
        }

        base_SafeHandle(base_SafeHandle const&) = delete;

        base_SafeHandle(base_SafeHandle&& other) noexcept
            : _value{other._value}
        {
            other._value = TraitsT::Invalid();
        }

        base_SafeHandle& operator=(base_SafeHandle const&) = delete;

        base_SafeHandle& operator=(base_SafeHandle&& other) noexcept
        {
            if (this != &other)
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

        [[nodiscard]] constexpr HandleT Get() const
        {
            return this->_value;
        }

        [[nodiscard]] HandleT const* GetAddressOf() const
        {
            return &this->_value;
        }

        [[nodiscard]] HandleT* GetAddressOf()
        {
            return &this->_value;
        }

    public:
        HandleT Detach()
        {
            HandleT const value = this->_value;
            this->_value = TraitsT::Invalid();
            return value;
        }

        void Attach(HandleT value)
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Close(this->_value);
            }

            this->_value = value;
        }

        void Reset()
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Close(this->_value);
                this->_value = TraitsT::Invalid();
            }
        }
    };
}
