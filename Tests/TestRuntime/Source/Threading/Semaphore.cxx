#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Threading/UserSemaphore.hxx"

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

        Worker workers[8]{
            {semaphore, counter, 1},
            {semaphore, counter, 2},
            {semaphore, counter, 3},
            {semaphore, counter, 4},
            {semaphore, counter, 5},
            {semaphore, counter, 6},
            {semaphore, counter, 7},
            {semaphore, counter, 8},
        };
        {
            Thread threads[8]{
                Thread{ThreadStart{{}, {}, {}, &workers[0]}},
                Thread{ThreadStart{{}, {}, {}, &workers[1]}},
                Thread{ThreadStart{{}, {}, {}, &workers[2]}},
                Thread{ThreadStart{{}, {}, {}, &workers[3]}},
                Thread{ThreadStart{{}, {}, {}, &workers[4]}},
                Thread{ThreadStart{{}, {}, {}, &workers[5]}},
                Thread{ThreadStart{{}, {}, {}, &workers[6]}},
                Thread{ThreadStart{{}, {}, {}, &workers[7]}},
            };
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

        Worker workers[8]{
            {semaphore, counter, 1},
            {semaphore, counter, 2},
            {semaphore, counter, 3},
            {semaphore, counter, 4},
            {semaphore, counter, 5},
            {semaphore, counter, 6},
            {semaphore, counter, 7},
            {semaphore, counter, 8},
        };
        {
            Thread threads[8]{
                Thread{ThreadStart{{}, {}, {}, &workers[0]}},
                Thread{ThreadStart{{}, {}, {}, &workers[1]}},
                Thread{ThreadStart{{}, {}, {}, &workers[2]}},
                Thread{ThreadStart{{}, {}, {}, &workers[3]}},
                Thread{ThreadStart{{}, {}, {}, &workers[4]}},
                Thread{ThreadStart{{}, {}, {}, &workers[5]}},
                Thread{ThreadStart{{}, {}, {}, &workers[6]}},
                Thread{ThreadStart{{}, {}, {}, &workers[7]}},
            };
        }

        constexpr int expected = 1000 * (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8);
        REQUIRE(counter == expected);
    }
}
