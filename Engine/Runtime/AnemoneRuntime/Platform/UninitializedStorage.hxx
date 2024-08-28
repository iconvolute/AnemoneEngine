#pragma once
#include <new>
#include <cstddef>
#include <memory>
#include <utility>

namespace Anemone::Platform
{
    template <typename T, size_t Size, size_t Alignment>
    struct UninitializedStorage
    {
        using UnderlyingType = T;

        alignas(Alignment) char Storage[Size];
    };

    template <typename T, size_t SizeT, size_t AlignmentT, typename... Args>
    constexpr T& Create(UninitializedStorage<T, SizeT, AlignmentT>& storage, Args&&... args)
    {
        static_assert(sizeof(T) <= SizeT, "Size of storage is too small for type T");
        static_assert(alignof(T) <= AlignmentT, "Alignment of storage is not compatible with type T");
        return *std::construct_at(reinterpret_cast<T*>(&storage.Storage), std::forward<Args>(args)...);
    }

    template <typename T, size_t SizeT, size_t AlignmentT>
    constexpr void Destroy(UninitializedStorage<T, SizeT, AlignmentT>& storage)
    {
        static_assert(sizeof(T) <= SizeT, "Size of storage is too small for type T");
        static_assert(alignof(T) <= AlignmentT, "Alignment of storage is not compatible with type T");
        std::destroy_at(reinterpret_cast<T*>(&storage.Storage));
    }


    template <typename T, size_t SizeT, size_t AlignmentT>
    constexpr T& Get(UninitializedStorage<T, SizeT, AlignmentT>& storage)
    {
        static_assert(sizeof(T) <= SizeT, "Size of storage is too small for type T");
        static_assert(alignof(T) <= AlignmentT, "Alignment of storage is not compatible with type T");
        return *std::launder<T>(reinterpret_cast<T*>(&storage.Storage));
    }

    template <typename T, size_t SizeT, size_t AlignmentT>
    constexpr T const& Get(UninitializedStorage<T, SizeT, AlignmentT> const& storage)
    {
        static_assert(sizeof(T) <= SizeT, "Size of storage is too small for type T");
        static_assert(alignof(T) <= AlignmentT, "Alignment of storage is not compatible with type T");
        return *std::launder<T const>(reinterpret_cast<T const*>(&storage.Storage));
    }
}
