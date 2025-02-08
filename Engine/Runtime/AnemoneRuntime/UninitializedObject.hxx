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
        bool m_initialized{false};

    public:
        UninitializedObject() = default;
        UninitializedObject(UninitializedObject const&) = delete;
        UninitializedObject(UninitializedObject&&) = delete;
        UninitializedObject& operator=(UninitializedObject const&) = delete;
        UninitializedObject& operator=(UninitializedObject&&) = delete;
        ~UninitializedObject() = default;

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->m_initialized;
        }

        [[nodiscard]] T& Get()
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] T const& Get() const
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] T* operator->()
        {
            return std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] T const* operator->() const
        {
            return std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] T& operator*()
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

        [[nodiscard]] T const& operator*() const
        {
            return *std::launder<T>(reinterpret_cast<T*>(this->_buffer));
        }

    public:
        [[nodiscard]] bool IsInitialized() const
        {
            return this->m_initialized;
        }

        template <typename... Args>
        void Create(Args&&... args)
        {
            if (this->m_initialized)
            {
                AE_PANIC("Object already initialized");
            }

            std::construct_at<T>(reinterpret_cast<T*>(this->_buffer), std::forward<Args>(args)...);
        }

        void Destroy()
        {
            if (this->m_initialized)
            {
                AE_PANIC("Object not initialized");
            }
            std::destroy_at(reinterpret_cast<T*>(this->_buffer));
        }
    };
}
