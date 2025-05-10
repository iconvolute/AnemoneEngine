#pragma once
#include <concepts>
#include <utility>
#include <atomic>

namespace Anemone
{
    template <typename T>
    class ReferenceTracker
    {
    private:
        std::atomic_uint32_t mutable m_ReferenceCount{0};

    private:
        ReferenceTracker() = default;
        ReferenceTracker(ReferenceTracker const&) = delete;
        ReferenceTracker(ReferenceTracker&&) = delete;
        ReferenceTracker& operator=(ReferenceTracker const&) = delete;
        ReferenceTracker& operator=(ReferenceTracker&&) = delete;
        ~ReferenceTracker() = default;

    public:
        uint32_t AcquireReference() const
        {
            return this->m_ReferenceCount.fetch_add(1, std::memory_order_relaxed);
        }

        uint32_t ReleaseReference() const
        {
            uint32_t const count = this->m_ReferenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1u;

            if (count == 0)
            {
                // The last reference is being released, delete the object
                // We use a static_cast here to avoid the need for a virtual destructor
                // This is safe because we know that T is derived from ReferenceTracker<T>
                // and thus has the same layout as T.
                // Note: This assumes that T does not have a virtual destructor.
                // If T does have a virtual destructor, this will not work correctly.
                // In that case, you should use a smart pointer or similar mechanism.

                delete static_cast<const T*>(this);
                return 0;
            }

            return count;
        }

        friend T;
    };

    template <typename T>
    class Reference
    {
        friend class ReferenceTracker<T>;

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
