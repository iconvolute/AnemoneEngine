#pragma once
#include <span>
#include <type_traits>

namespace Anemone::Interop
{
    template <typename HandleT, typename TraitsT>
    class SafeHandleT final
    {
    private:
        HandleT _value = TraitsT::Invalid();

    public:
        SafeHandleT() = default;

        explicit SafeHandleT(HandleT value)
            : _value{value}
        {
        }

        SafeHandleT(SafeHandleT const&) = delete;

        SafeHandleT(SafeHandleT&& other) noexcept
            : _value{other._value}
        {
            other._value = TraitsT::Invalid();
        }

        SafeHandleT& operator=(SafeHandleT const&) = delete;

        SafeHandleT& operator=(SafeHandleT&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->Attach(other.Detach());
            }

            return *this;
        }

        ~SafeHandleT()
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Reset(this->_value);
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
                TraitsT::Reset(this->_value);
            }

            this->_value = value;
        }

        void Reset()
        {
            if (TraitsT::IsValid(this->_value))
            {
                TraitsT::Reset(this->_value);
                this->_value = TraitsT::Invalid();
            }
        }
    };

    template <typename T, typename TraitsT>
    class SafeBufferT final
    {
    private:
        T* _data = TraitsT::Invalid();
        size_t _size = 0;

    public:
        SafeBufferT() = default;

        SafeBufferT(T* data, size_t size)
            : _data{data}
            , _size{size}
        {
        }

        SafeBufferT(SafeBufferT const&) = delete;

        SafeBufferT(SafeBufferT&& other) noexcept
            : _data{other._data}
            , _size{other._size}
        {
            other._data = TraitsT::Invalid();
            other._size = 0;
        }

        SafeBufferT& operator=(SafeBufferT const&) = delete;

        SafeBufferT& operator=(SafeBufferT&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                if (TraitsT::IsValid(this->_data))
                {
                    TraitsT::Reset(this->_data, this->_size);
                }

                this->_data = other._data;
                this->_size = other._size;

                other._data = TraitsT::Invalid();
                other._size = 0;
            }
            return *this;
        }

        ~SafeBufferT()
        {
            if (TraitsT::IsValid(this->_data))
            {
                TraitsT::Reset(this->_data, this->_size);
            }
        }

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->IsValid();
        }

        [[nodiscard]] constexpr bool IsValid() const
        {
            return TraitsT::IsValid(this->_data);
        }

        [[nodiscard]] T* GetData()
        {
            return this->_data;
        }

        [[nodiscard]] T const* GetData() const
        {
            return this->_data;
        }

        [[nodiscard]] size_t GetSize() const
        {
            return this->_size;
        }

        void Attach(void* data, size_t size)
        {
            if (TraitsT::IsValid(this->_data))
            {
                TraitsT::Reset(this->_data, this->_size);
            }

            this->_data = static_cast<T*>(data);
            this->_size = size;
        }

        void Detach(void*& data, size_t& size)
        {
            data = this->_data;
            size = this->_size;

            this->_data = TraitsT::Invalid();
            this->_size = 0;
        }

        void Reset()
        {
            if (TraitsT::IsValid(this->_data))
            {
                TraitsT::Reset(this->_data, this->_size);

                this->_data = TraitsT::Invalid();
                this->_size = 0;
            }
        }

        [[nodiscard]] std::span<std::byte> AsSpan()
        {
            return std::span<std::byte>(
                reinterpret_cast<std::byte*>(this->_data),
                this->_size);
        }

        [[nodiscard]] std::span<std::byte const> AsSpan() const
        {
            return std::span<std::byte const>(
                reinterpret_cast<std::byte const*>(this->_data),
                this->_size);
        }
    };
}
