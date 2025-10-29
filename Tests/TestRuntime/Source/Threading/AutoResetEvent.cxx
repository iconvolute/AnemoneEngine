#include "AnemoneThreading/Thread.hxx"
#include "AnemoneThreading/UserAutoResetEvent.hxx"
#include "AnemoneThreading/AutoResetEvent.hxx"
#include "AnemoneThreading/CurrentThread.hxx"
#include "AnemoneBase/Duration.hxx"

#include <catch_amalgamated.hpp>

#include <atomic>


TEST_CASE("Threading / UserAutoResetEvent")
{
    using namespace Anemone;

    struct SharedState
    {
        std::atomic_size_t PushSet{};
        std::atomic_size_t PushWait{};
        UserAutoResetEvent Push{false};
        std::atomic_size_t PopSet{};
        std::atomic_size_t PopWait{};
        UserAutoResetEvent Pop{false};

        std::atomic_uint64_t Checksum{};
    };

    struct Worker : public Runnable
    {
    private:
        SharedState& State;

    public:
        size_t Count;
        size_t Index;

    protected:
        void OnRun() override
        {
            for (size_t i = 0; i < this->Count; ++i)
            {
                this->State.Push.Wait();
                ++this->State.PushWait;

                this->State.Checksum.fetch_add(this->Index);

                this->State.Pop.Set();
                ++this->State.PopSet;
            }
        }

    public:
        Worker(SharedState& state, size_t count, size_t index)
            : State{state}
            , Count{count}
            , Index{index}
        {
        }
    };


    SharedState state;
    Reference<Worker> w1 = MakeReference<Worker>(state, 23uz, 0x1uz);
    Reference<Worker> w2 = MakeReference<Worker>(state, 24uz, 0x1'0000uz);
    Reference<Worker> w3 = MakeReference<Worker>(state, 25uz, 0x1'0000'0000uz);

    Reference<Thread> t1 = Thread::Start(ThreadStart{.Callback = w1});
    Reference<Thread> t2 = Thread::Start(ThreadStart{.Callback = w2});
    Reference<Thread> t3 = Thread::Start(ThreadStart{.Callback = w3});

    size_t const totalCount = w1->Count + w2->Count + w3->Count;

    for (size_t i = 0; i < totalCount; ++i)
    {
        ++state.PushSet;
        state.Push.Set();

        CurrentThread::Sleep(50);

        ++state.PopWait;
        state.Pop.Wait();
    }

    t1->Join();
    t2->Join();
    t3->Join();

    uint64_t const expectedChecksum = (w1->Count * w1->Index) + (w2->Count * w2->Index) + (w3->Count * w3->Index);

    CHECK(expectedChecksum == state.Checksum.load());

    CHECK_FALSE(state.Push.IsSignaled());
    CHECK_FALSE(state.Pop.IsSignaled());

    CHECK(state.PushSet == totalCount);
    CHECK(state.PushWait == totalCount);
    CHECK(state.PopSet == totalCount);
    CHECK(state.PopWait == totalCount);
}

TEST_CASE("Threading / AutoResetEvent")
{
    using namespace Anemone;

    struct SharedState
    {
        std::atomic_size_t PushSet{};
        std::atomic_size_t PushWait{};
        AutoResetEvent Push{false};
        std::atomic_size_t PopSet{};
        std::atomic_size_t PopWait{};
        AutoResetEvent Pop{false};

        std::atomic_uint64_t Checksum{};
    };

    struct Worker : public Runnable
    {
    private:
        SharedState& State;

    public:
        size_t Count;
        size_t Index;

    protected:
        void OnRun() override
        {
            for (size_t i = 0; i < this->Count; ++i)
            {
                this->State.Push.Wait();
                ++this->State.PushWait;

                this->State.Checksum.fetch_add(this->Index);

                this->State.Pop.Set();
                ++this->State.PopSet;
            }
        }

    public:
        Worker(SharedState& state, size_t count, size_t index)
            : State{state}
            , Count{count}
            , Index{index}
        {
        }
    };


    SharedState state;
    Reference<Worker> w1 = MakeReference<Worker>(state, 23uz, 0x1uz);
    Reference<Worker> w2 = MakeReference<Worker>(state, 24uz, 0x1'0000uz);
    Reference<Worker> w3 = MakeReference<Worker>(state, 25uz, 0x1'0000'0000uz);

    Reference<Thread> t1 = Thread::Start(ThreadStart{.Callback = w1});
    Reference<Thread> t2 = Thread::Start(ThreadStart{.Callback = w2});
    Reference<Thread> t3 = Thread::Start(ThreadStart{.Callback = w3});

    size_t const totalCount = w1->Count + w2->Count + w3->Count;

    for (size_t i = 0; i < totalCount; ++i)
    {
        ++state.PushSet;
        state.Push.Set();

        CurrentThread::Sleep(50);

        ++state.PopWait;
        state.Pop.Wait();
    }

    t1->Join();
    t2->Join();
    t3->Join();

    uint64_t const expectedChecksum = (w1->Count * w1->Index) + (w2->Count * w2->Index) + (w3->Count * w3->Index);

    CHECK(expectedChecksum == state.Checksum.load());

    // CHECK_FALSE(state.Push.IsSignaled());
    // CHECK_FALSE(state.Pop.IsSignaled());

    CHECK(state.PushSet == totalCount);
    CHECK(state.PushWait == totalCount);
    CHECK(state.PopSet == totalCount);
    CHECK(state.PopWait == totalCount);
}
