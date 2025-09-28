#pragma once
#include "AnemoneMemory/RawMemoryArena.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <string_view>
#include <span>
#include <memory>
#include <cstring>

namespace Anemone
{
    /// \brief General-purpose memory arena that allows allocation of objects with custom deleters.
    class MEMORY_API MemoryArena final
    {
    public:
        static constexpr size_t DefaultSegmentSize = RawMemoryArena::DefaultSegmentSize;

    private:
        using DeleterCallback = void(void* object, size_t count);

        struct Deleter
        {
            Deleter* Next{};
            DeleterCallback* Callback{};
            void* Object{};
            size_t Count{};
        };

    private:
        Deleter* _deleters{};

        RawMemoryArena _arena{};

    private:
        [[nodiscard]] void* RawAllocateWithDeleter(
            size_t size,
            size_t alignment,
            DeleterCallback deleterCallback,
            size_t count);

        [[nodiscard]] void* RawAllocate(
            size_t size,
            size_t alignment);

    public:
        MemoryArena()
            : MemoryArena{DefaultSegmentSize}
        {
        }

        explicit MemoryArena(size_t defaultSegmentSize)
            : _arena{defaultSegmentSize}
        {
        }

        MemoryArena(MemoryArena const&) = delete;

        MemoryArena(MemoryArena&&) = delete;

        MemoryArena& operator=(MemoryArena const&) = delete;

        MemoryArena& operator=(MemoryArena&&) = delete;

        ~MemoryArena()
        {
            this->Reset();
        }

    public:
        template <typename T, typename... Args>
        [[nodiscard]] T* Make(Args&&... args)
        {
            void* memory;

            if constexpr (std::is_trivially_destructible_v<T>)
            {
                // T is trivially destructible. Do not register any deleter
                memory = this->RawAllocate(sizeof(T), alignof(T));
            }
            else
            {
                // T is not trivially destructible. Register a deleter callback to call the destructor.
                memory = this->RawAllocateWithDeleter(
                    sizeof(T),
                    alignof(T),
                    [](void* object, size_t count)
                {
                    (void)count;
                    std::destroy_at(static_cast<T*>(object));
                },
                    1);
            }

            return std::construct_at(static_cast<T*>(memory), std::forward<Args>(args)...);
        }

        template <typename T>
        [[nodiscard]] std::span<T> MakeSpan(size_t count)
        {
            T* memory;

            if constexpr (std::is_trivially_destructible_v<T>)
            {
                memory = static_cast<T*>(this->RawAllocate(sizeof(T) * count, alignof(T)));
            }
            else
            {
                memory = static_cast<T*>(this->RawAllocateWithDeleter(
                    sizeof(T) * count,
                    alignof(T),
                    [](void* object, size_t count)
                {
                    T* ptr = static_cast<T*>(object);
                    std::destroy_n(ptr, count);
                },
                    count));
            }

            std::uninitialized_default_construct_n(memory, count);

            std::span<T> result{
                memory,
                count,
            };

            return result;
        }

        template <typename T>
        [[nodiscard]] std::span<T> MakeSpan(std::span<T const> source)
        {
            T* memory;

            if constexpr (std::is_trivially_destructible_v<T>)
            {
                memory = static_cast<T*>(this->RawAllocate(sizeof(T) * source.size(), alignof(T)));
            }
            else
            {
                memory = static_cast<T*>(this->RawAllocateWithDeleter(
                    sizeof(T) * source.size(),
                    alignof(T),
                    [](void* object, size_t count)
                {
                    T* ptr = static_cast<T*>(object);
                    std::destroy_n(ptr, count);
                },
                    source.size()));
            }

            std::uninitialized_copy_n(source.data(), source.size(), memory);

            std::span<T> result{
                memory,
                source.size(),
            };

            return result;
        }

        [[nodiscard]] std::string_view MakeString(std::string_view str)
        {
            char* const buffer = static_cast<char*>(this->_arena.Allocate(str.length() + 1, 1));
            AE_ASSERT(buffer != nullptr, "Out of memory");
            std::memcpy(buffer, str.data(), str.length());
            buffer[str.length()] = '\0'; // Null-terminate the string

            return std::string_view{buffer, str.length()};
        }

        void Reset();

        void QueryMemoryUsage(size_t& reserved, size_t& allocated) const
        {
            this->_arena.QueryMemoryUsage(reserved, allocated);
        }
    };
}
