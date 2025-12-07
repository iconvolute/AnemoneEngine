#include "AnemoneRuntime.Threading/SpscQueue.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END


TEST_CASE("SPSC Queue - Single Thread")
{
    using namespace Anemone;

    struct Tracking final
    {
        int Item;
        size_t* Constructors;
        size_t* Destructors;

        Tracking(int item, size_t* constructors, size_t* destructors) noexcept
            : Item{item}
            , Constructors{constructors}
            , Destructors{destructors}
        {
            ++(*this->Constructors);
        }

        Tracking(Tracking const&) = default;
        Tracking(Tracking&&) = default;
        Tracking& operator=(Tracking const&) = default;
        Tracking& operator=(Tracking&&) = default;

        ~Tracking() noexcept
        {
            ++(*this->Destructors);
        }
    };

    SECTION("Empty queue")
    {
        SpscQueue<Tracking> queue{4};

        {
            REQUIRE_FALSE(queue.TryPop(nullptr));
        }
    }

    SECTION("Single item")
    {
        size_t constructors{};
        size_t destructors{};

        SpscQueue<Tracking> queue{4};

        {
            REQUIRE(queue.TryEmplace(1, &constructors, &destructors));
        }

        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 1);
        }

        {
            REQUIRE_FALSE(queue.TryPop(nullptr));
        }

        REQUIRE(constructors == (1 * 2));
        REQUIRE(destructors == (1 * 2));
    }

    SECTION("Multiple items")
    {
        size_t constructors{};
        size_t destructors{};

        SpscQueue<Tracking> queue{4};

        {
            REQUIRE(queue.TryEmplace(1, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(2, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(3, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(4, &constructors, &destructors));
            REQUIRE_FALSE(queue.TryEmplace(5, &constructors, &destructors));
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 1);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 2);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 3);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 4);
        }
        {
            REQUIRE_FALSE(queue.TryPop(nullptr));
        }

        REQUIRE(constructors == (4 * 2));
        REQUIRE(destructors == (4 * 2));
    }

    SECTION("Push and process multiple times")
    {
        size_t constructors{};
        size_t destructors{};

        SpscQueue<Tracking> queue{4};

        {
            REQUIRE(queue.TryEmplace(1, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(2, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(3, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(4, &constructors, &destructors));
            REQUIRE_FALSE(queue.TryEmplace(5, &constructors, &destructors));
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 1);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 2);
        }
        {
            REQUIRE(queue.TryEmplace(5, &constructors, &destructors));
            REQUIRE(queue.TryEmplace(6, &constructors, &destructors));
            REQUIRE_FALSE(queue.TryEmplace(7, &constructors, &destructors));
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 3);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 4);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 5);
        }
        {
            Tracking item{0, &constructors, &destructors};
            REQUIRE(queue.TryPop(&item));
            REQUIRE(item.Item == 6);
        }
        {
            REQUIRE_FALSE(queue.TryPop(nullptr));
        }

        REQUIRE(constructors == (6 * 2));
        REQUIRE(destructors == (6 * 2));
    }
}
