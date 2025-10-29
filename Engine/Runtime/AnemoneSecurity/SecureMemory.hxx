#pragma once

namespace Anemone
{
    struct SecureMemory
    {
        static constexpr void* Zero(void* pointer, size_t size)
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

        template <typename T, size_t Extent>
        static constexpr void Zero(std::span<T, Extent> span)
        {
            Zero(span.data(), span.size_bytes());
        }
    };
}
