#pragma once
#include <type_traits>
#include <bit>

namespace Anemone::Reflection
{
    struct CxxMemoryLayout final
    {
        size_t size;
        size_t alignment;
    };

    using CxxFnInvokeDefaultConstructor = void (void* object);
    using CxxFnInvokeCopyConstructor = void(void* object, void const* source);
    using CxxFnInvokeMoveConstructor = void(void* object, void* source);
    using CxxFnInvokeCopyAssignment = void(void* object, void const* source);
    using CxxFnInvokeMoveAssignment = void(void* object, void* source);
    using CxxFnInvokeDestructor = void(void* object);

    struct CxxTypeTraits final
    {
        CxxFnInvokeDefaultConstructor* m_defaultConstructor{};
        CxxFnInvokeCopyConstructor* m_copyConstructor{};
        CxxFnInvokeMoveConstructor* m_moveConstructor{};
        CxxFnInvokeDestructor* m_destructor{};
        CxxFnInvokeCopyAssignment* m_copyAssignment{};
        CxxFnInvokeMoveAssignment* m_moveAssignment{};
        CxxMemoryLayout m_memoryLayout{};

        template <typename T>
        static constexpr CxxTypeTraits GetType()
        {
            CxxTypeTraits result{};

            if constexpr (std::is_default_constructible_v<T>)
            {
                result.m_defaultConstructor = +[](void* object)
                {
                    new (object) T();
                };
            }

            if constexpr (std::is_copy_constructible_v<T>)
            {
                result.m_copyConstructor = +[](void* object, void const* source)
                {
                    new (object) T(*std::bit_cast<T const*>(source));
                };
            }

            if constexpr (std::is_move_constructible_v<T>)
            {
                result.m_moveConstructor = +[](void* object, void* source)
                {
                    new (object) T(std::move(*std::bit_cast<T*>(source)));
                };
            }

            if constexpr (std::is_destructible_v<T>)
            {
                result.m_destructor = +[](void* object)
                {
                    std::bit_cast<T*>(object)->~T();
                };
            }

            if constexpr (std::is_copy_assignable_v<T>)
            {
                result.m_copyAssignment = +[](void* object, void const* source)
                {
                    *std::bit_cast<T*>(object) = *std::bit_cast<T const*>(source);
                };
            }

            if constexpr (std::is_move_assignable_v<T>)
            {
                result.m_moveAssignment = +[](void* object, void* source)
                {
                    *std::bit_cast<T*>(object) = std::move(*std::bit_cast<T*>(source));
                };
            }

            result.m_memoryLayout = CxxMemoryLayout{
                .size = sizeof(T),
                .alignment = alignof(T),
            };

            return result;
        }
    };

    
}
