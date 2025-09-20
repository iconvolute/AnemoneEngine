#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

#include <algorithm>
#include <cstring>
#include <algorithm>
#include <span>
#include <cstddef>
#include <expected>
#include <new>

namespace Anemone
{
    class MemoryBuffer final : public ReferenceCounted<MemoryBuffer>
    {
    private:
        std::byte* _data{};
        std::size_t _size{};
        std::size_t _capacity{}; // allocated bytes (for owned buffers)
        bool _owned{};

    private:
        MemoryBuffer(std::byte* data, std::size_t size, std::size_t capacity, bool owned)
            : _data{data}
            , _size{size}
            , _capacity{capacity}
            , _owned{owned}
        {
        }

    public:
        MemoryBuffer() = delete;

        MemoryBuffer(MemoryBuffer const&) = delete;

        MemoryBuffer(MemoryBuffer&&) = delete;

        MemoryBuffer& operator=(MemoryBuffer const&) = delete;

        MemoryBuffer& operator=(MemoryBuffer&&) = delete;

        ~MemoryBuffer()
        {
            if (this->_owned)
            {
                ::operator delete(this->_data);
            }
        }

    public:
        // creates memory buffer abstraction over non-owned memory block
        static std::expected<Reference<MemoryBuffer>, Status> CreateView(std::span<std::byte> buffer);

        // creates memory buffer with copy of the contents
        static std::expected<Reference<MemoryBuffer>, Status> Create(std::span<std::byte const> content);

        static std::expected<Reference<MemoryBuffer>, Status> Create(std::size_t size);

    private:
        // Growth policy: 1.5x current capacity or requested, whichever is larger.
        static std::size_t CalculateGrowth(std::size_t current, std::size_t requested);

    public:
        // Reserve at least the specified capacity, preserving size and contents.
        std::expected<void, Status> Reserve(std::size_t capacity);

        std::expected<void, Status> Resize(std::size_t size);

        std::expected<void, Status> Clear();

        // Reduce capacity to match current size (if owned). Frees unused memory.
        std::expected<void, Status> ShrinkToFit();

        // Gets view for whole memory buffer. Use spans for that
        [[nodiscard]] std::span<std::byte> GetView()
        {
            return std::span{this->_data, this->_size};
        }

        [[nodiscard]] std::span<std::byte const> GetView() const
        {
            return std::span{this->_data, this->_size};
        }

        [[nodiscard]] std::size_t GetCapacity() const
        {
            return this->_capacity;
        }

        [[nodiscard]] std::size_t GetSize() const
        {
            return this->_size;
        }

        [[nodiscard]] std::byte* GetData()
        {
            return this->_data;
        }

        [[nodiscard]] std::byte const* GetData() const
        {
            return this->_data;
        }

        [[nodiscard]] bool IsOwned() const
        {
            return this->_owned;
        }

        std::expected<void, Status> EnsureOwned();

        std::expected<Reference<MemoryBuffer>, Status> Clone() const;
    };
}
