#pragma once
#include "AnemoneDiagnostics/Debug.hxx"

#include <concepts>
#include <type_traits>
#include <cstddef>
#include <atomic>
#include <utility>

// Ideas:
// - Use special callback "OnLastReference" instead calling delete directly.

namespace Anemone
{
    template <typename Derived>
    class ReferenceCounted
    {
    private:
        mutable int32_t m_ReferenceCount{0};

    public:
        ReferenceCounted() = default;

        ReferenceCounted(ReferenceCounted const&) = delete;

        ReferenceCounted(ReferenceCounted&&) = delete;

        ReferenceCounted& operator=(ReferenceCounted const&) = delete;

        ReferenceCounted& operator=(ReferenceCounted&&) = delete;

#ifndef NDEBUG
        ~ReferenceCounted()
        {
            AE_ASSERT(this->m_ReferenceCount == 0, "Reference count must be zero on destruction");
        }
#else
        ~ReferenceCounted() = default;
#endif

    public:
        [[nodiscard]] constexpr size_t GetReferenceCount() const
        {
            return this->m_ReferenceCount;
        }

        constexpr int32_t AcquireReference() const
        {
            return ++this->m_ReferenceCount;
        }

        constexpr int32_t ReleaseReference() const
        {
            int32_t const expected = --this->m_ReferenceCount;

            AE_ASSERT(expected >= 0);

            if (expected == 0)
            {
                delete static_cast<Derived const*>(this);
            }

            return expected;
        }
    };

    template <typename Derived>
    class ThreadsafeReferenceCounted
    {
    private:
        mutable std::atomic<int32_t> m_ReferenceCount{0};

    public:
        ThreadsafeReferenceCounted() = default;

        ThreadsafeReferenceCounted(ThreadsafeReferenceCounted const&) = delete;

        ThreadsafeReferenceCounted(ThreadsafeReferenceCounted&&) = delete;

        ThreadsafeReferenceCounted& operator=(ThreadsafeReferenceCounted const&) = delete;

        ThreadsafeReferenceCounted& operator=(ThreadsafeReferenceCounted&&) = delete;

#ifndef NDEBUG
        ~ThreadsafeReferenceCounted()
        {
            AE_ASSERT(this->m_ReferenceCount.load() == 0, "Reference count must be zero on destruction");
        }
#else
        ~ThreadsafeReferenceCounted() = default;
#endif

    public:
        [[nodiscard]] constexpr size_t GetReferenceCount() const
        {
            return this->m_ReferenceCount.load(std::memory_order::relaxed);
        }

        constexpr int32_t AcquireReference() const
        {
            return this->m_ReferenceCount.fetch_add(1, std::memory_order::relaxed) + 1;
        }

        constexpr int32_t ReleaseReference() const
        {
            int32_t const expected = this->m_ReferenceCount.fetch_sub(1, std::memory_order::acq_rel) - 1;

            AE_ASSERT(expected >= 0);

            if (expected == 0)
            {
                delete static_cast<Derived const*>(this);
            }

            return expected;
        }
    };
}

namespace Anemone
{
    template <typename T>
    class [[nodiscard]] Reference final
    {
        template <typename U>
        friend class Reference;

    private:
        T* m_Object{};

    private:
        constexpr void AcquireReference()
        {
            if (this->m_Object)
            {
                this->m_Object->AcquireReference();
            }
        }

        constexpr void ReleaseReference()
        {
            if (this->m_Object)
            {
                this->m_Object->ReleaseReference();
            }
        }

    public:
        constexpr Reference() = default;

        explicit constexpr Reference(T* object)
            : m_Object{object}
        {
            this->AcquireReference();
        }

        constexpr Reference(Reference const& other)
            : m_Object{other.m_Object}
        {
            this->AcquireReference();
        }

        constexpr Reference(Reference&& other) noexcept
            : m_Object{other.m_Object}
        {
            other.m_Object = nullptr;
        }

        template <typename U>
            requires(std::is_convertible_v<U*, T*>)
        constexpr Reference(Reference<U> other)
            : m_Object{other.m_Object}
        {
            other.m_Object = nullptr;
        }

        constexpr ~Reference()
        {
            this->ReleaseReference();
        }

        constexpr Reference& operator=(Reference const& other)
        {
            if (this != std::addressof(other))
            {
                this->ReleaseReference();
                this->m_Object = other.m_Object;
                this->AcquireReference();
            }

            return *this;
        }

        constexpr Reference& operator=(Reference&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->ReleaseReference();
                this->m_Object = other.m_Object;
                other.m_Object = nullptr;
            }

            return *this;
        }

        [[nodiscard]] constexpr T& operator*() const
        {
            AE_ASSERT(this->m_Object != nullptr, "Dereferencing a null reference");
            return *this->m_Object;
        }

        [[nodiscard]] constexpr T* operator->() const
        {
            AE_ASSERT(this->m_Object != nullptr, "Dereferencing a null reference");
            return this->m_Object;
        }

        [[nodiscard]] constexpr T* Get() const
        {
            return this->m_Object;
        }

        [[nodiscard]] constexpr T** GetAddressOf() const
        {
            return std::addressof(this->m_Object);
        }

        [[nodiscard]] constexpr explicit operator bool() const
        {
            return this->m_Object != nullptr;
        }
    };

    template <class T, class U>
    [[nodiscard]] constexpr bool operator==(Reference<T> const& left, Reference<U> const& right)
    {
        return left.Get() == right.Get();
    }

    template <class T, class U>
    [[nodiscard]] constexpr bool operator!=(Reference<T> const& left, Reference<U> const& right)
    {
        return left.Get() != right.Get();
    }

    template <class T, class U>
    [[nodiscard]] constexpr bool operator==(Reference<T> const& left, U* right)
    {
        return left.Get() == right;
    }

    template <class T, class U>
    [[nodiscard]] constexpr bool operator!=(Reference<T> const& left, U* right)
    {
        return left.Get() != right;
    }

    template <class T, class U>
    [[nodiscard]] constexpr bool operator==(T* left, Reference<U> const& right)
    {
        return left == right.Get();
    }

    template <class T, class U>
    [[nodiscard]] constexpr bool operator!=(T* left, Reference<U> const& right)
    {
        return left != right.Get();
    }

    template <class T>
    [[nodiscard]] constexpr bool operator==(std::nullptr_t left, Reference<T> const& right)
    {
        return left == right.Get();
    }

    template <class T>
    [[nodiscard]] constexpr bool operator==(Reference<T> const& left, std::nullptr_t right)
    {
        return left.Get() == right;
    }

    template <class T>
    [[nodiscard]] constexpr bool operator!=(std::nullptr_t left, Reference<T> const& right)
    {
        return !(left == right);
    }

    template <class T>
    [[nodiscard]] constexpr bool operator!=(Reference<T> const& left, std::nullptr_t right)
    {
        return !(left == right);
    }

    template <typename T, typename... Args>
    [[nodiscard]] Reference<T> MakeReference(Args&&... args)
    {
        return Reference<T>(new T{std::forward<Args>(args)...});
    }
}
