#pragma once
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

#include <cstddef>
#include <memory>

namespace Anemone
{
    template <typename T>
    class Singleton final
    {
    private:
        alignas(T) std::byte m_buffer[sizeof(T)]{};
        T* m_instance{};

    public:
        Singleton() = default;
        Singleton(Singleton const&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton const&) = delete;
        Singleton& operator=(Singleton&&) = delete;
        ~Singleton() = default;

    public:
        [[nodiscard]] T& Get()
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return *this->m_instance;
        }

        [[nodiscard]] T const& Get() const
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return *this->m_instance;
        }

        [[nodiscard]] T* operator->()
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return this->m_instance;
        }

        [[nodiscard]] T const* operator->() const
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return this->m_instance;
        }

        [[nodiscard]] T& operator*()
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return *this->m_instance;
        }

        [[nodiscard]] T const& operator*() const
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            return *this->m_instance;
        }

    public:
        [[nodiscard]] bool IsCreated() const
        {
            return this->m_instance != nullptr;
        }

        template <typename... Args>
        void Create(Args&&... args)
        {
            AE_ASSERT(this->m_instance == nullptr, "Singleton already initialized");
            this->m_instance = std::construct_at<T>(reinterpret_cast<T*>(this->m_buffer), std::forward<Args>(args)...);
        }

        void Destroy()
        {
            AE_ASSERT(this->m_instance != nullptr, "Singleton not initialized");
            std::destroy_at(this->m_instance);
            this->m_instance = nullptr;
        }
    };
}
