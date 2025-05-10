#include "AnemoneRuntime/Base/HandleTable.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

#include <bit>

namespace Anemone
{
    struct HandleTable::Traits final
    {
        static constexpr size_t TableShift = 8;
        static constexpr size_t TableSize = size_t{1} << TableShift;

        static constexpr uint64_t EncodeKey = 0x7f94a11ab7b3ee6bu;

        static constexpr Handle Encode(uint64_t value)
        {
            return Handle{value ^ EncodeKey};
        }

        static constexpr uint64_t Decode(Handle handle)
        {
            return handle.Value ^ EncodeKey;
        }

        static constexpr Handle Make(uint32_t index, uint32_t generation)
        {
            return Encode(static_cast<uint64_t>(index) | (static_cast<uint64_t>(generation) << 32u));
        }

        static constexpr uint32_t GetIndex(Handle handle)
        {
            return static_cast<uint32_t>(Decode(handle));
        }

        static constexpr uint32_t GetGeneration(Handle handle)
        {
            return static_cast<uint32_t>(Decode(handle) >> 32u);
        }

        static constexpr size_t ComputeLevelFromCapacity(size_t capacity)
        {
            if (capacity < TableSize)
            {
                return 0;
            }

            if (capacity < TableSize * TableSize)
            {
                return 1;
            }

            if (capacity < TableSize * TableSize * TableSize)
            {
                return 2;
            }

            if (capacity < TableSize * TableSize * TableSize * TableSize)
            {
                return 3;
            }

            std::unreachable();
        }

        static size_t ComputeMappingIndex(size_t index, size_t level)
        {
            return (index >> (TableShift * level)) & (TableSize - 1);
        }
    };

    struct HandleTable::Entry final
    {
        //
        // The data associated with the handle.
        //

        void* Data;

        //
        // Index of the next free entry in the handle table.
        //

        uint32_t Next;

        //
        // Generation of the handle.
        //

        uint32_t Generation;
    };

    struct HandleTable::EntryTable final
    {
        Entry Items[Traits::TableSize];
    };

    struct HandleTable::MappingTable final
    {
        void* Items[Traits::TableSize];
    };

    HandleTable::Entry* HandleTable::Lookup(uint32_t index) const
    {
        void* table = this->_root;

        switch (this->_level)
        {
        case 3:
            AE_ASSERT(table != nullptr);
            table = static_cast<MappingTable*>(table)->Items[Traits::ComputeMappingIndex(index, 3)];
            [[fallthrough]];

        case 2:
            AE_ASSERT(table != nullptr);
            table = static_cast<MappingTable*>(table)->Items[Traits::ComputeMappingIndex(index, 2)];
            [[fallthrough]];

        case 1:
            AE_ASSERT(table != nullptr);
            table = static_cast<MappingTable*>(table)->Items[Traits::ComputeMappingIndex(index, 1)];
            [[fallthrough]];

        case 0:
            AE_ASSERT(table != nullptr);
            return &static_cast<EntryTable*>(table)->Items[Traits::ComputeMappingIndex(index, 0)];

        default:
            break;
        }

        AE_ASSERT(false);
        std::unreachable();
    }

    void HandleTable::DeallocateRecursive(void* table, size_t level)
    {
        if (level == 0)
        {
            delete static_cast<EntryTable*>(table);
        }
        else
        {
            MappingTable* mapping = static_cast<MappingTable*>(table);

            for (auto& entry : mapping->Items)
            {
                if (entry != nullptr)
                {
                    DeallocateRecursive(entry, level - 1);
                }
            }

            delete mapping;
        }
    }

    void HandleTable::DeallocateTables()
    {
        if (_root != nullptr)
        {
            DeallocateRecursive(_root, _level);
            _root = nullptr;
        }
    }

    HandleTable::MappingTable* HandleTable::EnsureMappingTable(MappingTable* parent, size_t slot)
    {
        if (parent->Items[slot] == nullptr)
        {
            parent->Items[slot] = new MappingTable{};
        }

        return static_cast<MappingTable*>(parent->Items[slot]);
    }

    bool HandleTable::AllocateEntryTable()
    {
        size_t const requiredLevel = Traits::ComputeLevelFromCapacity(this->_capacity);

        //
        // Ensure tree height for given capacity.
        //

        while (this->_level < requiredLevel)
        {
            MappingTable* newRoot = new MappingTable{};
            newRoot->Items[0] = this->_root;
            this->_root = newRoot;
            ++this->_level;
        }

        EntryTable* entryTable = new EntryTable;

        if (this->_root == nullptr)
        {
            //
            // For _level == 0, the new EntryTable becomes the root.
            //

            this->_root = entryTable;
            this->_level = 0;
        }
        else
        {
            //
            // For _level > 0, descend the tree to find the appropriate slot for the new EntryTable.
            //

            MappingTable* mapping = static_cast<MappingTable*>(this->_root);

            for (size_t currentLevel = this->_level; currentLevel > 1; --currentLevel)
            {
                size_t const slot = Traits::ComputeMappingIndex(this->_count, currentLevel);
                mapping = EnsureMappingTable(mapping, slot);
            }

            //
            // At level 1, compute the slot using the lower bits.
            //

            size_t const slot = Traits::ComputeMappingIndex(this->_count, 1);
            mapping->Items[slot] = entryTable;
        }


        //
        // Update capacity.
        //

        this->_capacity += Traits::TableSize;


        //
        // Initialize the free list pointer based on _count.
        //

        this->_first = static_cast<uint32_t>(this->_count);

        uint32_t next = this->_first;

        uintptr_t address = std::bit_cast<uintptr_t>(entryTable);

        uint32_t generation = static_cast<uint32_t>(address >> 32u) ^ static_cast<uint32_t>(address);

        //
        // Initialize the free list links in the new EntryTable.
        //

        for (size_t i = 0; i < Traits::TableSize - 1; ++i)
        {
            ++next;

            entryTable->Items[i] = Entry{nullptr, next, generation};

            generation *= 6269629u;
        }

        entryTable->Items[Traits::TableSize - 1] = Entry{nullptr, InvalidIndex, generation};

        return true;
    }

    std::optional<HandleTable::Handle> HandleTable::Allocate()
    {
        if (this->_first == InvalidIndex)
        {
            //
            // No free entries available in the current table.
            //

            if (not this->AllocateEntryTable())
            {
                return std::nullopt;
            }
        }

        //
        // Unlink entry from the free list.
        //

        uint32_t index = this->_first;
        Entry* entry = this->Lookup(index);

        this->_first = entry->Next;
        entry->Next = InvalidIndex;
        entry->Data = nullptr;

        ++this->_count;

        return Traits::Make(index, entry->Generation);
    }

    bool HandleTable::Deallocate(Handle handle)
    {
        //
        // Lookup entry from handle.
        //

        uint32_t index = Traits::GetIndex(handle);
        Entry* entry = this->Lookup(index);

        AE_ASSERT(entry->Generation == Traits::GetGeneration(handle));
        AE_ASSERT(entry->Next == InvalidIndex);

        if ((entry->Generation == Traits::GetGeneration(handle)) and (entry->Next == InvalidIndex))
        {
            //
            // Link entry to the free list.
            //

            ++entry->Generation;
            --this->_count;

            entry->Next = this->_first;
            this->_first = index;

            return true;
        }

        return false;
    }

    std::optional<void*> HandleTable::Get(Handle handle) const
    {
        //
        // Lookup entry from handle.
        //

        uint32_t index = Traits::GetIndex(handle);
        Entry* entry = this->Lookup(index);

        if ((entry->Generation == Traits::GetGeneration(handle)) and (entry->Next == InvalidIndex))
        {
            //
            // Return the data associated with the handle.
            //

            return entry->Data;
        }

        return std::nullopt;
    }

    bool HandleTable::Set(Handle handle, void* data)
    {
        //
        // Lookup entry from handle.
        //

        uint32_t index = Traits::GetIndex(handle);
        Entry* entry = this->Lookup(index);

        if ((entry->Generation == Traits::GetGeneration(handle)) and (entry->Next == InvalidIndex))
        {
            //
            // Update the data associated with the handle.
            //

            entry->Data = data;
            return true;
        }

        return false;
    }
}
