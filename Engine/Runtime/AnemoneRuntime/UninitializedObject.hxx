#pragma once
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <cstddef>
#include <memory>

namespace Anemone
{
    // TODO: Rename this to more descriptive name
    template <typename T>
    class UninitializedObject final
    {
    private:
        alignas(T) std::byte _buffer[sizeof(T)]{};
        T* m_instance = nullptr;

    public:
        UninitializedObject() = default;
        UninitializedObject(UninitializedObject const&) = delete;
        UninitializedObject(UninitializedObject&&) = delete;
        UninitializedObject& operator=(UninitializedObject const&) = delete;
        UninitializedObject& operator=(UninitializedObject&&) = delete;
        ~UninitializedObject() = default;

    public:
        [[nodiscard]] explicit constexpr operator bool() const
        {
            return this->m_instance != nullptr;
        }

        [[nodiscard]] constexpr T& Get()
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] constexpr T const& Get() const
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] constexpr T* operator->()
        {
            return std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] constexpr T const* operator->() const
        {
            return std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] constexpr T& operator*()
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] constexpr T const& operator*() const
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

    public:
        [[nodiscard]] constexpr bool IsInitialized() const
        {
            return this->m_instance != nullptr;
        }

        template <typename... Args>
        constexpr void Create(Args&&... args)
        {
            if (this->m_instance)
            {
                AE_PANIC("Object already initialized");
            }

            this->m_instance = std::construct_at<T>(reinterpret_cast<T*>(this->_buffer), std::forward<Args>(args)...);
        }

        constexpr void Destroy()
        {
            if (not this->m_instance)
            {
                AE_PANIC("Object not initialized");
            }

            std::destroy_at(reinterpret_cast<T*>(this->_buffer));

            this->m_instance = nullptr;
        }
    };
}
