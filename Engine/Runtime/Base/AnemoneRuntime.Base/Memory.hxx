#pragma once
#include <cstddef>
#include <type_traits>

namespace Anemone
{
    constexpr void* secure_clear_memory(void* pointer, size_t size)
    {
        volatile char* it = static_cast<volatile char*>(pointer);
        volatile char* end = it + size;

        for (; it != end; ++it)
        {
#if defined(_MSC_VER)
            __iso_volatile_store8(it, 0);
#else
            *it = 0;
#endif
        }

        return pointer;
    }

    template <typename T>
    [[nodiscard]] T volatile_load(T const* location) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);
        return *static_cast<T const volatile*>(location);
    }

    template <typename T>
    void volatile_store(T* location, T value) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>);
        *static_cast<T volatile*>(location) = value;
    }
}
