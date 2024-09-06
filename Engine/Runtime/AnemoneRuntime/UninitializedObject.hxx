#pragma once
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

#include <cstddef>
#include <memory>

namespace Anemone
{
    template <typename T>
    class UninitializedObject final
    {
    private:
        alignas(T) std::byte _buffer[sizeof(T)]{};
        T* _instance = nullptr;

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
            return this->_instance != nullptr;
        }

        [[nodiscard]] T& Get()
        {
            return *this->_instance;
        }

        [[nodiscard]] T const& Get() const
        {
            return *this->_instance;
        }

        [[nodiscard]] T* operator->()
        {
            return this->_instance;
        }

        [[nodiscard]] T const* operator->() const
        {
            return this->_instance;
        }

        [[nodiscard]] T& operator*()
        {
            return *this->_instance;
        }

        [[nodiscard]] T const& operator*() const
        {
            return *this->_instance;
        }

    public:
        bool IsInitialized() const
        {
            return this->_instance != nullptr;
        }

        template <typename... Args>
        void Create(Args&&... args)
        {
            if (this->_instance != nullptr)
            {
                AE_PANIC("Object already initialized");
            }

            this->_instance = std::construct_at<T>(reinterpret_cast<T*>(this->_buffer), std::forward<Args>(args)...);
        }

        void Destroy()
        {
            if (this->_instance == nullptr)
            {
                AE_PANIC("Object not initialized");
            }
            std::destroy_at(this->_instance);
            this->_instance = nullptr;
        }
    };
}
