#include "AnemoneRuntime.Base/LockfreeIntrusive.hxx"
#include "AnemoneRuntime.Tasks/Parallel.hxx"

#include <catch_amalgamated.hpp>

namespace
{
    struct TestNode : private Anemone::LockFreeIntrusiveStackNode<TestNode>
    {
        friend class Anemone::LockFreeIntrusiveStack<TestNode>;

        size_t Value{};

        explicit TestNode(size_t value)
            : Value{value}
        {
        }
    };
}

TEST_CASE("Lock Free - Stack")
{
    std::vector<TestNode*> nodes{};
    constexpr size_t nodeCount = 12345;

    for (size_t i = 0; i < nodeCount; ++i)
    {
        nodes.emplace_back(new TestNode(i));
    }

    Anemone::LockFreeIntrusiveStack<TestNode> stack{};

    Anemone::Parallel::For(nodeCount, 32, [&](size_t index, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            stack.Push(nodes[index + i]);
        }
    });

    size_t sum = 0;

    while (TestNode* node = stack.Pop())
    {
        sum += node->Value;
        delete node;
    }

    constexpr size_t expectedSum = (nodeCount * (nodeCount - 1)) / 2;

    REQUIRE(sum == expectedSum);
}
