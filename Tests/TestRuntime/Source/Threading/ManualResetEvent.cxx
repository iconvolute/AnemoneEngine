#include "AnemoneRuntime/Threading/Thread.hxx"
#include "AnemoneRuntime/Threading/ManualResetEventSlim.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <catch_amalgamated.hpp>

#include <atomic>


TEST_CASE("Threading / ManualResetEvent")
{
    using namespace Anemone::Threading;
    using namespace Anemone;

    struct SharedState
    {
        std::atomic_size_t BeforeWait{};
        std::atomic_size_t MiddleWait{};
        std::atomic_size_t AfterWait{};

        ManualResetEventSlim Start{false};
        ManualResetEventSlim Middle1{false};
        ManualResetEventSlim Middle2{false};
        ManualResetEventSlim End{false};

        std::atomic_uint64_t Checksum{};
    };

    struct Worker : public Runnable
    {
    public:
        SharedState& State;
        ManualResetEventSlim BeforeWait;
        ManualResetEventSlim MiddleWait;
        ManualResetEventSlim AfterWait;
    protected:
        void OnRun() override
        {
            this->State.Start.Wait();

            ++this->State.BeforeWait;
            this->BeforeWait.Set();

            this->State.Middle1.Wait();

            ++this->State.MiddleWait;
            this->MiddleWait.Set();

            this->State.Middle2.Wait();

            ++this->State.AfterWait;
            this->AfterWait.Set();

            this->State.End.Wait();
        }

    public:
        [[nodiscard]] Worker(SharedState& state)
            : State{state}
        {
        }
    };


    SharedState state;
    Worker w1{state};
    Worker w2{state};
    Worker w3{state};

    Thread t1{ThreadStart{.Callback = &w1}};
    Thread t2{ThreadStart{.Callback = &w2}};
    Thread t3{ThreadStart{.Callback = &w3}};

    CHECK(state.BeforeWait == 0);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Start.Set();

    w1.BeforeWait.Wait();
    w2.BeforeWait.Wait();
    w3.BeforeWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Middle1.Set();

    w1.MiddleWait.Wait();
    w2.MiddleWait.Wait();
    w3.MiddleWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 0);

    state.Middle2.Set();

    w1.AfterWait.Wait();
    w2.AfterWait.Wait();
    w3.AfterWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);

    state.End.Set();

    t1.Join();
    t2.Join();
    t3.Join();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);
}
