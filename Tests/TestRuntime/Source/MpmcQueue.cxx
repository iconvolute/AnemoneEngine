#include "AnemoneRuntime/Threading/MpmcQueue.hxx"
#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("MPMC Queue - Multiple Threads")
{
    using namespace Anemone;

    struct Context final
    {
        std::atomic_size_t Constructors{};
        std::atomic_size_t Destructors{};
        std::atomic_size_t Consumed{};
        std::atomic_size_t Produced{};
        std::atomic_size_t ConsumedChecksum{};
        std::atomic_size_t ProducedChecksum{};
        std::atomic_size_t Finished{};
        size_t Count{100};
    };

    struct Tracking final
    {
        size_t _item{};
        Context* _context{};

        Tracking()
        {
        }

        Tracking(size_t item, Context& context)
            : _item{item}
            , _context{&context}
        {
            ++this->_context->Constructors;
        }

        Tracking(Tracking const&) = delete;

        Tracking& operator=(Tracking const&) = delete;

        Tracking(Tracking&& other) noexcept
            : _item{std::exchange(other._item, {})}
            , _context{std::exchange(other._context, {})}
        {
        }

        Tracking& operator=(Tracking&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->_item = std::exchange(other._item, {});
                this->_context = std::exchange(other._context, {});
            }

            return *this;
        }

        ~Tracking()
        {
            if (this->_context)
            {
                ++this->_context->Destructors;
            }
        }

        void Consume()
        {
            ++this->_context->Consumed;
            this->_context->ConsumedChecksum += this->_item;
        }
    };

    MpmcQueue<Tracking> queue{4};

    struct Consumer final : Runnable
    {
    private:
        MpmcQueue<Tracking>& _queue;
        Context* _context;

    protected:
        void OnRun() override
        {
            bool finish = false;
            do
            {
                if (this->_context->Finished.load(std::memory_order::acquire) != 0)
                {
                    finish = true;
                }

                Tracking item{};

                if (this->_queue.TryPop(&item))
                {
                    // Consume item
                    item.Consume();
                }
                else if (finish)
                {
                    break;
                }
                else
                {
                    // Yield(ThreadYieldTarget::SameOrHigherPriorityOnAnyProcessor);
                }

            } while (true);
        }

    public:
        Consumer(MpmcQueue<Tracking>& queue, Context& context)
            : _queue{queue}
            , _context{&context}
        {
        }
    };

    struct Producer final : Runnable
    {
    private:
        MpmcQueue<Tracking>& _queue;
        Context* _context;

    protected:
        void OnRun() override
        {
            for (size_t i = 0; i < this->_context->Count;)
            {
                if (this->_queue.TryEmplace(i, *this->_context))
                {
                    ++this->_context->Produced;
                    this->_context->ProducedChecksum += i;
                    ++i;
                }
                else
                {
                    // Sleep(Duration::FromMilliseconds(50));
                }
            }
        }

    public:
        Producer(MpmcQueue<Tracking>& queue, Context& context)
            : _queue{queue}
            , _context{&context}
        {
        }
    };

    SECTION("Single Producer - Single Consumer")
    {
        Context context{};

        Producer producer1{queue, context};
        Consumer consumer1{queue, context};

        Thread consumerThread1{ThreadStart{.Name = "Consumer 1", .Callback = &consumer1}};
        Thread producerThread1{ThreadStart{.Name = "Producer 1", .Callback = &producer1}};

        producerThread1.Join();
        context.Finished.store(1, std::memory_order::release);

        consumerThread1.Join();

        REQUIRE(context.Consumed == context.Count);
        REQUIRE(context.Produced == context.Count);
        REQUIRE(context.Constructors == context.Count);
        REQUIRE(context.Destructors == context.Count);
        REQUIRE(context.ConsumedChecksum == (context.Count * (context.Count - 1)) / 2);
        REQUIRE(context.ProducedChecksum == context.ConsumedChecksum);
    }

    SECTION("Multiple Producer - Single Consumer")
    {
        Context context{};

        Producer producer1{queue, context};
        Producer producer2{queue, context};
        Producer producer3{queue, context};
        Producer producer4{queue, context};

        Consumer consumer1{queue, context};

        Thread consumerThread1{ThreadStart{.Name = "Consumer", .Callback = &consumer1}};
        Thread producerThread1{ThreadStart{.Name = "Producer 1", .Callback = &producer1}};
        Thread producerThread2{ThreadStart{.Name = "Producer 2", .Callback = &producer2}};
        Thread producerThread3{ThreadStart{.Name = "Producer 3", .Callback = &producer3}};
        Thread producerThread4{ThreadStart{.Name = "Producer 4", .Callback = &producer4}};

        producerThread1.Join();
        producerThread2.Join();
        producerThread3.Join();
        producerThread4.Join();
        context.Finished.store(1, std::memory_order::release);
        consumerThread1.Join();

        size_t const totalProduced = context.Count * 4;
        size_t const checksum = (context.Count * (context.Count - 1)) / 2;

        REQUIRE(context.Consumed == totalProduced);
        REQUIRE(context.Produced == totalProduced);
        REQUIRE(context.Constructors == totalProduced);
        REQUIRE(context.Destructors == totalProduced);
        REQUIRE(context.ConsumedChecksum == checksum * 4);
        REQUIRE(context.ProducedChecksum == context.ConsumedChecksum);
    }

    SECTION("Multiple Producer - Multiple Consumer")
    {
        Context context{};

        Producer producer1{queue, context};
        Producer producer2{queue, context};
        Producer producer3{queue, context};
        Producer producer4{queue, context};
        Producer producer5{queue, context};

        Consumer consumer1{queue, context};
        Consumer consumer2{queue, context};
        Consumer consumer3{queue, context};

        Thread consumerThread1{ThreadStart{.Name = "Consumer 1", .Callback = &consumer1}};
        Thread consumerThread2{ThreadStart{.Name = "Consumer 2", .Callback = &consumer2}};
        Thread consumerThread3{ThreadStart{.Name = "Consumer 3", .Callback = &consumer3}};

        Thread producerThread1{ThreadStart{.Name = "Producer 1", .Callback = &producer1}};
        Thread producerThread2{ThreadStart{.Name = "Producer 2", .Callback = &producer2}};
        Thread producerThread3{ThreadStart{.Name = "Producer 3", .Callback = &producer3}};
        Thread producerThread4{ThreadStart{.Name = "Producer 4", .Callback = &producer4}};
        Thread producerThread5{ThreadStart{.Name = "Producer 5", .Callback = &producer5}};

        producerThread1.Join();
        producerThread2.Join();
        producerThread3.Join();
        producerThread4.Join();
        producerThread5.Join();
        context.Finished.store(1, std::memory_order::release);
        consumerThread1.Join();
        consumerThread2.Join();
        consumerThread3.Join();

        size_t const totalProduced = context.Count * 5;
        size_t const checksum = (context.Count * (context.Count - 1)) / 2;

        REQUIRE(context.Consumed == totalProduced);
        REQUIRE(context.Produced == totalProduced);
        REQUIRE(context.Constructors == totalProduced);
        REQUIRE(context.Destructors == totalProduced);
        REQUIRE(context.ConsumedChecksum == checksum * 5);
        REQUIRE(context.ProducedChecksum == context.ConsumedChecksum);
    }

    SECTION("Single Producer - Multiple Consumer")
    {
        Context context{};

        Producer producer1{queue, context};

        Consumer consumer1{queue, context};
        Consumer consumer2{queue, context};
        Consumer consumer3{queue, context};

        Thread consumerThread1{ThreadStart{.Name = "Consumer 1", .Callback = &consumer1}};
        Thread consumerThread2{ThreadStart{.Name = "Consumer 2", .Callback = &consumer2}};
        Thread consumerThread3{ThreadStart{.Name = "Consumer 3", .Callback = &consumer3}};

        Thread producerThread1{ThreadStart{.Name = "Producer 1", .Callback = &producer1}};

        producerThread1.Join();
        context.Finished.store(1, std::memory_order::release);
        consumerThread1.Join();
        consumerThread2.Join();
        consumerThread3.Join();

        size_t const totalProduced = context.Count * 1;
        size_t const checksum = (context.Count * (context.Count - 1)) / 2;

        REQUIRE(context.Consumed == totalProduced);
        REQUIRE(context.Produced == totalProduced);
        REQUIRE(context.Constructors == totalProduced);
        REQUIRE(context.Destructors == totalProduced);
        REQUIRE(context.ConsumedChecksum == checksum * 1);
        REQUIRE(context.ProducedChecksum == context.ConsumedChecksum);
    }
}

TEST_CASE("MPMC Queue - Single Thread")
{
    using namespace Anemone;

    struct Tracking final
    {
        int Item;
        size_t* Constructors;
        size_t* Destructors;

        Tracking(int item, size_t* constructors, size_t* destructors)
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

        ~Tracking()
        {
            ++(*this->Destructors);
        }
    };

    SECTION("Empty queue")
    {
        MpmcQueue<Tracking> queue{4};

        {
            REQUIRE_FALSE(queue.TryPop(nullptr));
        }
    }

    SECTION("Single item")
    {
        size_t constructors{};
        size_t destructors{};

        MpmcQueue<Tracking> queue{4};

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

        MpmcQueue<Tracking> queue{4};

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

        MpmcQueue<Tracking> queue{4};

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
