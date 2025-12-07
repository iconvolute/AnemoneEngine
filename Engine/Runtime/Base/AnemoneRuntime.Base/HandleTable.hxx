#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <optional>
#include <utility>

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API HandleTable
    {
        static constexpr uint32_t InvalidIndex = 0xFFFFFFFFu;

    private:
        size_t _level = 0;
        size_t _capacity = 0;
        size_t _count = 0;
        uint32_t _first = InvalidIndex;
        void* _root = nullptr;

    public:
        struct Handle final
        {
            uint64_t Value;
        };

    public:
        HandleTable() = default;

        HandleTable(HandleTable const&) = delete;

        HandleTable& operator=(HandleTable const&) = delete;

        HandleTable(HandleTable&& other) noexcept
            : _level{std::exchange(other._level, 0)}
            , _capacity{std::exchange(other._capacity, 0)}
            , _count{std::exchange(other._count, 0)}
            , _first{std::exchange(other._first, InvalidIndex)}
            , _root{std::exchange(other._root, nullptr)}
        {
        }

        HandleTable& operator=(HandleTable&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->DeallocateTables();

                this->_level = std::exchange(other._level, 0);
                this->_capacity = std::exchange(other._capacity, 0);
                this->_count = std::exchange(other._count, 0);
                this->_first = std::exchange(other._first, InvalidIndex);
                this->_root = std::exchange(other._root, nullptr);
            }

            return *this;
        }

        ~HandleTable()
        {
            this->DeallocateTables();
        }

    private:
        struct Traits;

        struct Entry;

        struct EntryTable;

        struct MappingTable;

        Entry* Lookup(uint32_t index) const;

        static void DeallocateRecursive(void* table, size_t level);

        void DeallocateTables();

        static MappingTable* EnsureMappingTable(MappingTable* parent, size_t slot);

        bool AllocateEntryTable();

    public:
        size_t Count() const
        {
            return this->_count;
        }

        size_t Capacity() const
        {
            return this->_capacity;
        }

        std::optional<Handle> Allocate();

        bool Deallocate(Handle handle);

        std::optional<void*> Get(Handle handle) const;

        bool Set(Handle handle, void* data);
    };
}
