#include "AnemoneRuntime.Threading/Thread.hxx"
#include "AnemoneRuntime.Threading/Semaphore.hxx"
#include "AnemoneRuntime.Threading/UserSemaphore.hxx"

#include <catch_amalgamated.hpp>

//
// Each test case spawn N threads, where each thread increments a shared counter by its own value.
// Threads are synchronized using a semaphore. No atomics are involved.
//

TEST_CASE("Threading / Semaphore")
{
    SECTION("Semaphore")
    {
        using namespace Anemone;

        class Worker : public Runnable
        {
        private:
            Semaphore& _semaphore;
            int& _counter;
            int _value;

        public:
            Worker(
                Semaphore& semaphore,
                int& counter,
                int value)
                : _semaphore{semaphore}
                , _counter{counter}
                , _value{value}
            {
            }

            void OnRun() override
            {
                for (int i = 0; i < 1000; ++i)
                {
                    this->_semaphore.Acquire();
                    this->_counter += this->_value;
                    this->_semaphore.Release();
                }
            }
        };

        Semaphore semaphore{1};
        int counter{0};

        Reference<Worker> workers[8]{
            MakeReference<Worker>(semaphore, counter, 1),
            MakeReference<Worker>(semaphore, counter, 2),
            MakeReference<Worker>(semaphore, counter, 3),
            MakeReference<Worker>(semaphore, counter, 4),
            MakeReference<Worker>(semaphore, counter, 5),
            MakeReference<Worker>(semaphore, counter, 6),
            MakeReference<Worker>(semaphore, counter, 7),
            MakeReference<Worker>(semaphore, counter, 8),
        };
        {
            Reference<Thread> threads[8]{
                Thread::Start(ThreadStart{{}, {}, {}, workers[0]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[1]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[2]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[3]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[4]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[5]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[6]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[7]}),
            };

            for (Reference<Thread>& thread : threads)
            {
                thread->Join();
            }
        }

        constexpr int expected = 1000 * (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);
        REQUIRE(counter == expected);
    }

    SECTION("UserSemaphore")
    {
        using namespace Anemone;

        class Worker : public Runnable
        {
        private:
            UserSemaphore& _semaphore;
            int& _counter;
            int _value;

        public:
            Worker(
                UserSemaphore& semaphore,
                int& counter,
                int value)
                : _semaphore{semaphore}
                , _counter{counter}
                , _value{value}
            {
            }

            void OnRun() override
            {
                for (int i = 0; i < 1000; ++i)
                {
                    this->_semaphore.Acquire();
                    this->_counter += this->_value;
                    this->_semaphore.Release();
                }
            }
        };

        UserSemaphore semaphore{1};
        int counter{0};

        Reference<Worker> workers[8]{
            MakeReference<Worker>(semaphore, counter, 1),
            MakeReference<Worker>(semaphore, counter, 2),
            MakeReference<Worker>(semaphore, counter, 3),
            MakeReference<Worker>(semaphore, counter, 4),
            MakeReference<Worker>(semaphore, counter, 5),
            MakeReference<Worker>(semaphore, counter, 6),
            MakeReference<Worker>(semaphore, counter, 7),
            MakeReference<Worker>(semaphore, counter, 8),
        };
        {
            Reference<Thread> threads[8]{
                Thread::Start(ThreadStart{{}, {}, {}, workers[0]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[1]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[2]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[3]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[4]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[5]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[6]}),
                Thread::Start(ThreadStart{{}, {}, {}, workers[7]}),
            };

            for (Reference<Thread>& thread : threads)
            {
                thread->Join();
            }
        }

        constexpr int expected = 1000 * (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);
        REQUIRE(counter == expected);
    }
}
