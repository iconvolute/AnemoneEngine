#include "AnemoneMemory/MemoryArena.hxx"
#include "AnemoneBase/Bitwise.hxx"

namespace Anemone
{
    void* MemoryArena::RawAllocateWithDeleter(
        size_t size,
        size_t alignment,
        DeleterCallback deleterCallback,
        size_t count)
    {
        AE_ASSERT(size != 0);
        AE_ASSERT(alignment != 0);
        AE_ASSERT(deleterCallback != nullptr);
        AE_ASSERT(count != 0);

        // Ensure that the alignment is at least as strict as the deleter's alignment.
        size_t const totalAlignment = std::max(alignof(Deleter), alignment);

        // Calculate the offset to the allocated object.
        size_t const allocationOffset = AlignUp(sizeof(Deleter), totalAlignment);

        // Calculate total size of the allocation for deleter and the object.
        size_t const totalSize = AlignUp(allocationOffset + size, totalAlignment);

        // Allocate memory from the arena.
        void* const allocation = this->_arena.Allocate(totalSize, totalAlignment);
        AE_ASSERT(allocation != nullptr, "Out of memory");

        // Calculate the pointer to the object within the allocation.
        void* const result = Adjust(allocation, static_cast<ptrdiff_t>(allocationOffset));

        // Construct and remember deleter for that object.
        this->_deleters = std::construct_at(static_cast<Deleter*>(allocation), this->_deleters, deleterCallback, result, count);

        return result;
    }

    void* MemoryArena::RawAllocate(
        size_t size,
        size_t alignment)
    {
        return this->_arena.Allocate(size, alignment);
    }

    void MemoryArena::Reset()
    {
        // Invoke all registered deleters.
        Deleter* current = this->_deleters;
        this->_deleters = nullptr;

        while (current)
        {
            current->Callback(current->Object, current->Count);
            current = current->Next;
        }

        // Then, release memory.
        this->_arena.Reset();
    }
}
