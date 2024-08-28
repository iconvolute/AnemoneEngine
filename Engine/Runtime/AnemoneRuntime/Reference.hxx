#pragma once

#include <compare>
#include <concepts>
#include <utility>

namespace Anemone
{
    template <typename T>
    class Reference
    {
    private:
        T* m_Instance{};

    public:
        Reference() = default;

        Reference(T* instance)
            : m_Instance{instance}
        {
            if (this->m_Instance)
            {
                this->m_Instance->AcquireReference();
            }
        }

        Reference(Reference const& other)
            : m_Instance{other.m_Instance}
        {
            if (this->m_Instance)
            {
                this->m_Instance->AcquireReference();
            }
        }

        Reference(Reference&& other) noexcept
            : m_Instance{std::exchange(other.m_Instance, nullptr)}
        {
        }

        Reference& operator=(Reference const& other)
        {
            if (this != std::addressof(other))
            {
                if (this->m_Instance != other.m_Instance)
                {
                    if (other.m_Instance)
                    {
                        other.m_Instance->AcquireReference();
                    }

                    if (this->m_Instance)
                    {
                        this->m_Instance->ReleaseReference();
                    }

                    this->m_Instance = other.m_Instance;
                }
            }

            return *this;
        }

        Reference& operator=(Reference&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                if (this->m_Instance)
                {
                    this->m_Instance->ReleaseReference();
                }

                this->m_Instance = std::exchange(other.m_Instance, nullptr);
            }

            return *this;
        }

        ~Reference()
        {
            if (this->m_Instance)
            {
                this->m_Instance->ReleaseReference();
            }
        }

        [[nodiscard]] friend constexpr bool operator==(Reference const& self, Reference const& other)
        {
            return self.m_Instance == other.m_Instance;
        }

        [[nodiscard]] friend constexpr bool operator!=(Reference const& self, Reference const& other)
        {
            return !(self == other);
        }

        [[nodiscard]] friend constexpr bool operator==(Reference const& self, std::nullptr_t)
        {
            return self.m_Instance == nullptr;
        }

        [[nodiscard]] friend constexpr bool operator!=(Reference const& self, std::nullptr_t)
        {
            return self.m_Instance != nullptr;
        }

        [[nodiscard]] friend constexpr bool operator==(std::nullptr_t, Reference const& other)
        {
            return other.m_Instance == nullptr;
        }

        [[nodiscard]] friend constexpr bool operator!=(std::nullptr_t, Reference const& other)
        {
            return other.m_Instance != nullptr;
        }

        [[nodiscard]] T* operator->() const
        {
            return this->m_Instance;
        }

        [[nodiscard]] T& operator*() const
        {
            return *this->m_Instance;
        }

        [[nodiscard]] explicit operator bool() const
        {
            return this->m_Instance != nullptr;
        }

        [[nodiscard]] T* Get() const
        {
            return this->m_Instance;
        }

        [[nodiscard]] T* Detach()
        {
            return std::exchange(this->m_Instance, nullptr);
        }
    };
}
