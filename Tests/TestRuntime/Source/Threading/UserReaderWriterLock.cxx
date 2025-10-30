#include "AnemoneThreading/Thread.hxx"
#include "AnemoneThreading/UserReaderWriterLock.hxx"
#include "AnemoneThreading/CurrentThread.hxx"

#include <catch_amalgamated.hpp>
#include <atomic>
#include <thread>
#include <vector>

TEST_CASE("Threading / UserReaderWriterLock")
{
    using namespace Anemone;

    SECTION("Basic functionality")
    {
        UserReaderWriterLock lock;

        // Reader lock basic test
        REQUIRE_NOTHROW(lock.EnterShared());
        REQUIRE_NOTHROW(lock.LeaveShared());

        // Writer lock basic test
        REQUIRE_NOTHROW(lock.Enter());
        REQUIRE_NOTHROW(lock.Leave());

        // TryEnter tests
        REQUIRE(lock.TryEnter() == true);
        REQUIRE(lock.TryEnterShared() == false); // Can't get shared when exclusive is held
        lock.Leave();

        REQUIRE(lock.TryEnterShared() == true);
        REQUIRE(lock.TryEnter() == false); // Can't get exclusive when shared is held
        lock.LeaveShared();
    }

    SECTION("Concurrent readers")
    {
        UserReaderWriterLock lock;
        std::atomic<int> counter{};
        std::atomic<int> errors{};
        constexpr int numThreads = 10;
        std::vector<std::thread> threads;

        threads.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([&]()
            {
                lock.EnterShared();
                ++counter;
                // Simulate some work
                CurrentThread::Sleep(10);

                if (counter < 1)
                {
                    ++errors;
                }
                --counter;
                lock.LeaveShared();
            });
        }

        for (auto& t : threads)
        {
            t.join();
        }

        REQUIRE(errors == 0);
        REQUIRE(counter == 0);
    }

    SECTION("Writer exclusivity")
    {
        UserReaderWriterLock lock;
        std::atomic<bool> writerActive{false};
        std::atomic<int> activeReaders{0};
        std::atomic<int> totalErrors{0};

        constexpr int numReaders = 5;
        constexpr int numWriters = 3;
        std::vector<std::thread> threads;

        // Launch writer threads
        threads.reserve(numWriters + numReaders);
        for (int i = 0; i < numWriters; ++i)
        {
            threads.emplace_back([&]()
            {
                int errors{};

                for (int j = 0; j < 5; ++j)
                {
                    lock.Enter();

                    // Verify exclusivity
                    if (writerActive || activeReaders > 0)
                    {
                        ++errors;
                    }

                    writerActive = true;
                    CurrentThread::Sleep(1); // Small delay to increase chance of conflict
                    writerActive = false;

                    lock.Leave();

                    CurrentThread::Sleep(1); // Small delay between operations
                }

                totalErrors += errors;
            });
        }

        // Launch reader threads
        for (int i = 0; i < numReaders; ++i)
        {

            threads.emplace_back([&]()
            {
                int errors{};

                for (int j = 0; j < 10; ++j)
                {
                    lock.EnterShared();

                    // Verify no writer is active when we are reading
                    ++activeReaders;
                    if (writerActive)
                    {
                        ++errors;
                    }

                    CurrentThread::Sleep(1); // Small delay to increase chance of conflict

                    if (writerActive)
                    {
                        ++errors;
                    }
                    --activeReaders;

                    lock.LeaveShared();

                    CurrentThread::Sleep(1); // Small delay between operations
                }

                totalErrors += errors;
            });
        }

        for (auto& t : threads)
        {
            t.join();
        }

        REQUIRE(totalErrors == 0);
        REQUIRE(activeReaders == 0);
        REQUIRE(writerActive == false);
    }

    SECTION("RAII style usage")
    {
        UserReaderWriterLock lock;
        bool executed = false;

        // Test with exclusive lock
        {
            auto result = lock.With([&]()
            {
                executed = true;
                return 42;
            });

            REQUIRE(executed == true);
            REQUIRE(result == 42);
        }

        executed = false;

        // Test with shared lock
        {
            auto result = lock.WithShared([&]()
            {
                executed = true;
                return 43;
            });

            REQUIRE(executed == true);
            REQUIRE(result == 43);
        }
    }

    SECTION("Stress test")
    {
        UserReaderWriterLock lock;
        std::atomic<size_t> readers{};
        std::atomic<size_t> writers{};

        constexpr size_t numThreads = 20uz;
        constexpr size_t iterations = 1000uz;

        std::vector<std::thread> threads;

        threads.reserve(numThreads);

        for (size_t currentThread = 0uz; currentThread < numThreads; ++currentThread)
        {
            threads.emplace_back([&, id = currentThread]()
            {
                for (size_t currentIteration = 0uz; currentIteration < iterations; ++currentIteration)
                {
                    // Mix of different operations based on thread ID
                    if (id % 4 == 0)
                    {
                        // Writer
                        lock.With([&]
                        {
                            ++writers;
                        });
                    }
                    else
                    {
                        // Reader
                        lock.WithShared([&]
                        {
                            ++readers;
                        });
                    }
                }
            });
        }

        for (auto& t : threads)
        {
            t.join();
        }

        constexpr size_t expectedTotal = numThreads * iterations;
        constexpr size_t expectedWriters = expectedTotal / 4;
        constexpr size_t expectedReaders = expectedTotal - expectedWriters;

        REQUIRE(readers == expectedReaders);
        REQUIRE(writers == expectedWriters);
    }
}
