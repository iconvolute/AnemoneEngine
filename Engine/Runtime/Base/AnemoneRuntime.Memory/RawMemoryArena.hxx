#pragma once
#include "AnemoneRuntime.Base/Intrusive.hxx"

namespace Anemone
{
    class ANEMONE_RUNTIME_BASE_API RawMemoryArena final
    {
    public:
        static constexpr size_t DefaultSegmentSize = size_t{64} << 10u;

    private:
        struct Segment final : IntrusiveListNode<Segment>
        {
            size_t Size{};
            size_t Last{sizeof(Segment)};

            explicit Segment(size_t size)
                : Size{size}
            {
            }

            void* Allocate(size_t size, size_t alignment);

            [[nodiscard]] constexpr size_t GetAvailable() const
            {
                return this->Size - this->Last;
            }

            [[nodiscard]] constexpr size_t GetAllocated() const
            {
                return this->Last;
            }
        };

        struct Oversized final : IntrusiveListNode<Oversized>
        {
            size_t Size{};
            size_t Alignment{};

            explicit Oversized(size_t size, size_t alignment)
                : Size{size}
                , Alignment{alignment}
            {
            }
        };

    private:
        IntrusiveList<Segment> _segments{};
        IntrusiveList<Oversized> _oversized{};
        size_t _defaultSegmentSize{DefaultSegmentSize};

    private:
        Segment* AllocateSegment(size_t segmentSize);

        void* AllocateOversized(size_t size, size_t alignment);

    public:
        RawMemoryArena()
            : RawMemoryArena{DefaultSegmentSize}
        {
        }

        explicit RawMemoryArena(size_t defaultSegmentSize)
            : _defaultSegmentSize{defaultSegmentSize}
        {
        }

        RawMemoryArena(RawMemoryArena const&) = delete;

        RawMemoryArena(RawMemoryArena&&) = delete;

        RawMemoryArena& operator=(RawMemoryArena const&) = delete;

        RawMemoryArena& operator=(RawMemoryArena&&) = delete;

        ~RawMemoryArena()
        {
            this->Reset();
        }

    public:
        [[nodiscard]] void* Allocate(size_t size, size_t alignment);

        // Resets the arena, releasing all allocated memory.
        void Reset();

        void QueryMemoryUsage(size_t& reserved, size_t& allocated) const;
    };
}
