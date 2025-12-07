#include "AnemoneRuntime.Threading/Thread.hxx"
#include "AnemoneRuntime.Threading/UserManualResetEvent.hxx"
#include "AnemoneRuntime.Threading/ManualResetEvent.hxx"
#include "AnemoneRuntime.Threading/CurrentThread.hxx"
#include "AnemoneRuntime.Base/Duration.hxx"

#include <catch_amalgamated.hpp>

#include <atomic>


TEST_CASE("Threading / UserManualResetEvent")
{
    using namespace Anemone;

    struct SharedState
    {
        std::atomic_size_t BeforeWait{};
        std::atomic_size_t MiddleWait{};
        std::atomic_size_t AfterWait{};

        UserManualResetEvent Start{false};
        UserManualResetEvent Middle1{false};
        UserManualResetEvent Middle2{false};
        UserManualResetEvent End{false};

        std::atomic_uint64_t Checksum{};
    };

    struct Worker : public Runnable
    {
    public:
        SharedState& State;
        UserManualResetEvent BeforeWait;
        UserManualResetEvent MiddleWait;
        UserManualResetEvent AfterWait;

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
    Reference<Worker> w1 = MakeReference<Worker>(state);
    Reference<Worker> w2 = MakeReference<Worker>(state);
    Reference<Worker> w3 = MakeReference<Worker>(state);

    Reference<Thread> t1 = Thread::Start(ThreadStart{.Callback = w1});
    Reference<Thread> t2 = Thread::Start(ThreadStart{.Callback = w2});
    Reference<Thread> t3 = Thread::Start(ThreadStart{.Callback = w3});

    CHECK(state.BeforeWait == 0);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Start.Set();

    w1->BeforeWait.Wait();
    w2->BeforeWait.Wait();
    w3->BeforeWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Middle1.Set();

    w1->MiddleWait.Wait();
    w2->MiddleWait.Wait();
    w3->MiddleWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 0);

    state.Middle2.Set();

    w1->AfterWait.Wait();
    w2->AfterWait.Wait();
    w3->AfterWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);

    state.End.Set();

    t1->Join();
    t2->Join();
    t3->Join();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);
}

TEST_CASE("Threading / ManualResetEvent")
{
    using namespace Anemone;

    struct SharedState
    {
        std::atomic_size_t BeforeWait{};
        std::atomic_size_t MiddleWait{};
        std::atomic_size_t AfterWait{};

        ManualResetEvent Start{false};
        ManualResetEvent Middle1{false};
        ManualResetEvent Middle2{false};
        ManualResetEvent End{false};

        std::atomic_uint64_t Checksum{};
    };

    struct Worker : public Runnable
    {
    public:
        SharedState& State;
        ManualResetEvent BeforeWait;
        ManualResetEvent MiddleWait;
        ManualResetEvent AfterWait;

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
    Reference<Worker> w1 = MakeReference<Worker>(state);
    Reference<Worker> w2 = MakeReference<Worker>(state);
    Reference<Worker> w3 = MakeReference<Worker>(state);

    Reference<Thread> t1 = Thread::Start(ThreadStart{.Callback = w1});
    Reference<Thread> t2 = Thread::Start(ThreadStart{.Callback = w2});
    Reference<Thread> t3 = Thread::Start(ThreadStart{.Callback = w3});

    CHECK(state.BeforeWait == 0);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Start.Set();

    w1->BeforeWait.Wait();
    w2->BeforeWait.Wait();
    w3->BeforeWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 0);
    CHECK(state.AfterWait == 0);

    state.Middle1.Set();

    w1->MiddleWait.Wait();
    w2->MiddleWait.Wait();
    w3->MiddleWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 0);

    state.Middle2.Set();

    w1->AfterWait.Wait();
    w2->AfterWait.Wait();
    w3->AfterWait.Wait();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);

    state.End.Set();

    t1->Join();
    t2->Join();
    t3->Join();

    CHECK(state.BeforeWait == 3);
    CHECK(state.MiddleWait == 3);
    CHECK(state.AfterWait == 3);
}
