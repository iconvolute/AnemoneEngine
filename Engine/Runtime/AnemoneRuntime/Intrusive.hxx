#pragma once
#include "AnemoneRuntime/Diagnostic/Debug.hxx"

#include <type_traits>
#include <utility>

namespace Anemone
{
    template <typename T, typename Tag = void>
    struct IntrusiveSingleListNode
    {
        IntrusiveSingleListNode<T, Tag>* Link;
    };

    template <typename T, typename Tag = void>
    struct IntrusiveListNode
    {
        IntrusiveListNode<T, Tag>* FLink{};
        IntrusiveListNode<T, Tag>* BLink{};
    };
}

namespace Anemone
{
    template <typename T, typename Tag = void>
    struct IntrusiveStack
    {
    public:
        using Node = IntrusiveSingleListNode<T, Tag>;

    private:
        Node* Head = nullptr;

    public:
        constexpr IntrusiveStack() = default;

        static constexpr Node* AsNode(T* node)
        {
            return static_cast<Node*>(node);
        }

        static constexpr T* AsValue(Node* node)
        {
            return static_cast<T*>(node);
        }

        constexpr void Push(T* item)
        {
            Node* node = AsNode(item);
            node->Link = this->Head;
            this->Head = node;
        }

        constexpr T* Pop()
        {
            Node* node = this->Head;

            if (node)
            {
                this->Head = node->Link;
                node->Link = nullptr;
            }

            return AsValue(node);
        }

        constexpr void Clear()
        {
            this->Head = nullptr;
        }

        template <typename CallbackT = void(T&)>
        constexpr void ForEach(CallbackT&& callback) const
        {
            Node* node = this->Head;

            while (node)
            {
                std::forward<CallbackT>(callback)(*AsValue(node));
                node = node->Link;
            }
        }

        template <typename CallbackT = bool(T&)>
        constexpr T* Find(CallbackT&& callback) const
        {
            Node* node = this->Head;

            while (node)
            {
                T* value = AsValue(node);

                if (std::forward<CallbackT>(callback)(*value))
                {
                    return value;
                }

                node = node->Link;
            }

            return nullptr;
        }

        [[nodiscard]] constexpr bool IsEmpty() const
        {
            return this->Head == nullptr;
        }

        constexpr bool Splice(IntrusiveStack& other)
        {
            if (this != std::addressof(other))
            {
                if (this->IsEmpty() and not other.IsEmpty())
                {
                    this->Head = std::exchange(other.Head, nullptr);
                    return true;
                }
            }

            return false;
        }
    };
}

namespace Anemone
{
    template <typename T, typename Tag = void>
    struct IntrusiveSingleList
    {
    public:
        using Node = IntrusiveSingleListNode<T, Tag>;

    private:
        Node* Head = nullptr;
        Node* Tail = nullptr;

    public:
        constexpr IntrusiveSingleList() = default;

        static constexpr Node* AsNode(T* node)
        {
            return static_cast<Node*>(node);
        }

        static constexpr T* AsValue(Node* node)
        {
            return static_cast<T*>(node);
        }

        constexpr void PushFront(T* item)
        {
            Node* node = AsNode(item);
            node->Link = this->Head;
            this->Head = node;
            if (!this->Tail)
            {
                this->Tail = node;
            }
        }

        constexpr void PushBack(T* item)
        {
            Node* node = AsNode(item);
            node->Link = nullptr;

            if (this->Tail)
            {
                this->Tail->Link = node;
            }

            this->Tail = node;

            if (!this->Head)
            {
                this->Head = node;
            }
        }

        constexpr T* PopFront()
        {
            Node* node = this->Head;

            if (node)
            {
                this->Head = node->Link;

                if (node == this->Tail)
                {
                    this->Tail = nullptr;
                }

                node->Link = nullptr;
            }

            return AsValue(node);
        }

        constexpr void Clear()
        {
            this->Head = nullptr;
            this->Tail = nullptr;
        }

        template <typename CallbackT = void(T&)>
        constexpr void ForEach(CallbackT&& callback) const
        {
            Node* node = this->Head;

            while (node)
            {
                std::forward<CallbackT>(callback)(*AsValue(node));
                node = node->Link;
            }
        }

        template <typename CallbackT = bool(T&)>
        constexpr T* Find(CallbackT&& callback) const
        {
            Node* node = this->Head;

            while (node)
            {
                T* value = AsValue(node);

                if (std::forward<CallbackT>(callback)(*value))
                {
                    return value;
                }

                node = node->Link;
            }

            return nullptr;
        }

        [[nodiscard]] constexpr bool IsEmpty() const
        {
            return this->Head == nullptr;
        }

        constexpr bool Splice(IntrusiveSingleList& other)
        {
            if (this != std::addressof(other))
            {
                if (this->IsEmpty() and not other.IsEmpty())
                {
                    this->Head = std::exchange(other.Head, nullptr);
                    this->Tail = std::exchange(other.Tail, nullptr);
                    return true;
                }
            }

            return false;
        }
    };
}

namespace Anemone
{
    template <typename T, typename Tag = void>
    struct IntrusiveList
    {
    public:
        using Node = IntrusiveListNode<T, Tag>;

    private:
        Node Head{&Head, &Head};

    public:
        constexpr IntrusiveList() = default;

        static constexpr Node* AsNode(T* node)
        {
            return static_cast<Node*>(node);
        }

        static constexpr T* AsValue(Node* node)
        {
            return static_cast<T*>(node);
        }

        constexpr void PushFront(T* item)
        {
            Node* node = AsNode(item);
            AE_ASSERT(node->FLink == nullptr);
            AE_ASSERT(node->BLink == nullptr);
            node->FLink = this->Head.FLink;
            this->Head.FLink->BLink = node;
            node->BLink = &this->Head;
            this->Head.FLink = node;
        }

        constexpr void PushBack(T* item)
        {
            Node* node = AsNode(item);
            AE_ASSERT(node->FLink == nullptr);
            AE_ASSERT(node->BLink == nullptr);

            node->BLink = this->Head.BLink;
            this->Head.BLink->FLink = node;
            node->FLink = &this->Head;
            this->Head.BLink = node;
        }

        constexpr T* PopFront()
        {
            Node* node = this->Head.FLink;
            if (node == &this->Head)
            {
                // List is empty.
                return nullptr;
            }

            node->FLink->BLink = &this->Head;
            this->Head.FLink = node->FLink;
            node->FLink = nullptr;
            node->BLink = nullptr;
            return AsValue(node);
        }

        constexpr T* PopBack()
        {
            Node* node = this->Head.BLink;
            if (node == &this->Head)
            {
                // List is empty.
                return nullptr;
            }

            node->BLink->FLink = &this->Head;
            this->Head.BLink = node->BLink;
            node->FLink = nullptr;
            node->BLink = nullptr;
            return AsValue(node);
        }

        constexpr T* PeekFront()
        {
            Node* node = this->Head.FLink;
            if (node == &this->Head)
            {
                // List is empty.
                return nullptr;
            }

            return AsValue(node);
        }

        constexpr T* PeekBack()
        {
            Node* node = this->Head.BLink;
            if (node == &this->Head)
            {
                // List is empty.
                return nullptr;
            }

            return AsValue(node);
        }

        static constexpr void Remove(T* item)
        {
            Node* node = AsNode(item);

            Node* flink = node->FLink;
            Node* blink = node->BLink;

            flink->BLink = node->BLink;
            blink->FLink = node->FLink;

            item->FLink = nullptr;
            item->BLink = nullptr;
        }

        constexpr void Clear()
        {
            this->Head.FLink = &this->Head;
            this->Head.BLink = &this->Head;
        }

        template <typename CallbackT = void(T&)>
        constexpr void ForEach(CallbackT&& callback) const
        {
            Node* node = this->Head.FLink;
            while (node != &this->Head)
            {
                std::forward<CallbackT>(callback)(*AsValue(node));
                node = node->FLink;
            }
        }

        template <typename CallbackT = void(T&)>
        constexpr void ForEachReverse(CallbackT&& callback) const
        {
            Node* node = this->Head.BLink;
            while (node != &this->Head)
            {
                std::forward<CallbackT>(callback)(*AsValue(node));
                node = node->BLink;
            }
        }

        template <typename CallbackT = bool(T&)>
        constexpr T* Find(CallbackT&& callback) const
        {
            Node* node = this->Head.FLink;
            while (node != &this->Head)
            {
                T* value = AsValue(node);
                if (std::forward<CallbackT>(callback)(*value))
                {
                    return value;
                }
                node = node->FLink;
            }
            return nullptr;
        }

        [[nodiscard]] constexpr bool IsEmpty() const
        {
            return this->Head.FLink == &this->Head;
        }

        constexpr bool Splice(IntrusiveList& other)
        {
            if (this != std::addressof(other))
            {
                if (this->IsEmpty() and not other.IsEmpty())
                {
                    // Get elements from other list.
                    Node* flink = other.Head.FLink;
                    Node* blink = other.Head.BLink;

                    // Link them to this list.
                    flink->BLink = &this->Head;
                    blink->FLink = &this->Head;

                    // Clear other list.
                    other.Head.FLink = &other.Head;
                    other.Head.BLink = &other.Head;

                    // Link this list to the new elements.
                    this->Head.FLink = flink;
                    this->Head.BLink = blink;
                    return true;
                }
            }

            return false;
        }
    };
}
