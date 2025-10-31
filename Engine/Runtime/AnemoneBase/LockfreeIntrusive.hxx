#pragma once
#include <atomic>

namespace Anemone
{
    template<typename T, typename Tag = void>
    class LockFreeIntrusiveStack;

    template <typename T, typename Tag = void>
    class LockFreeIntrusiveStackNode
    {
        friend class LockFreeIntrusiveStack<T, Tag>;
    private:
        LockFreeIntrusiveStackNode* Next{};

    public:
        LockFreeIntrusiveStackNode() = default;
        LockFreeIntrusiveStackNode(LockFreeIntrusiveStackNode const&) = delete;
        LockFreeIntrusiveStackNode(LockFreeIntrusiveStackNode&&) = delete;
        LockFreeIntrusiveStackNode& operator=(LockFreeIntrusiveStackNode const&) = delete;
        LockFreeIntrusiveStackNode& operator=(LockFreeIntrusiveStackNode&&) = delete;
        ~LockFreeIntrusiveStackNode() = default;
    };

    template <typename T, typename Tag>
    class LockFreeIntrusiveStack
    {
    public:
        using Node = LockFreeIntrusiveStackNode<T, Tag>;

    private:
        struct Pointer
        {
            Node* value{};
            size_t tag{};
        };

    private:
        std::atomic<Pointer> _head{};

    private:
        static constexpr Node* AsNode(T* node)
        {
            return static_cast<Node*>(node);
        }

        static constexpr T* AsValue(Node* node)
        {
            return static_cast<T*>(node);
        }

    public:
        constexpr LockFreeIntrusiveStack() = default;

        LockFreeIntrusiveStack(LockFreeIntrusiveStack const&) = delete;

        LockFreeIntrusiveStack(LockFreeIntrusiveStack&&) = delete;

        constexpr ~LockFreeIntrusiveStack()
        {
            while (this->Pop())
            {
            }
        }

        LockFreeIntrusiveStack& operator=(LockFreeIntrusiveStack const&) = delete;

        LockFreeIntrusiveStack& operator=(LockFreeIntrusiveStack&&) = delete;

        constexpr void Push(T* item)
        {
            Pointer oldHead = this->_head.load(std::memory_order::relaxed);
            Pointer newHead{};

            do
            {
                Node* node = AsNode(item);
                node->Next = oldHead.value;
                newHead.value = node;
                newHead.tag = oldHead.tag + 1;
            } while (not this->_head.compare_exchange_weak(
                oldHead,
                newHead,
                std::memory_order::release,
                std::memory_order::relaxed));
        }

        constexpr T* Pop()
        {
            Pointer oldHead = this->_head.load(std::memory_order::acquire);
            Pointer newHead{};

            while (true)
            {
                Node* node = oldHead.value;

                if (not node)
                {
                    return nullptr; // empty
                }

                newHead.value = node->Next;
                newHead.tag = oldHead.tag + 1;

                if (this->_head.compare_exchange_weak(
                        oldHead,
                        newHead,
                        std::memory_order::acquire,
                        std::memory_order::relaxed))
                {
                    return AsValue(node);
                }
            }
        }

        [[nodiscard]] constexpr bool IsEmpty() const
        {
            return this->_head.load(std::memory_order::acquire).value == nullptr;
        }
    };
}
