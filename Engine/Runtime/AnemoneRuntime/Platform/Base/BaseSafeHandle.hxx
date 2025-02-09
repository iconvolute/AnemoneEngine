#pragma once
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

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
    class base_SafeBuffer final
    {
    private:
        T* _data = TraitsT::Invalid();
        size_t _size = 0;

    public:
        base_SafeBuffer() = default;

        base_SafeBuffer(T* data, size_t size)
            : _data{data}
            , _size{size}
        {
        }

        base_SafeBuffer(base_SafeBuffer const&) = delete;

        base_SafeBuffer(base_SafeBuffer&& other) noexcept
            : _data{other._data}
            , _size{other._size}
        {
            other._data = TraitsT::Invalid();
            other._size = 0;
        }

        base_SafeBuffer& operator=(base_SafeBuffer const&) = delete;

        base_SafeBuffer& operator=(base_SafeBuffer&& other) noexcept
        {
            if (this != &other)
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

        ~base_SafeBuffer()
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
    };
}
