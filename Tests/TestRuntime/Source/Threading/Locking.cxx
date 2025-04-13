#include "AnemoneRuntime/Tasks/Parallel.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Threading/Spinlock.hxx"
#include "AnemoneRuntime/Threading/UserCriticalSection.hxx"
#include "AnemoneRuntime/Threading/UserReaderWriterLock.hxx"
#include "AnemoneRuntime/Threading/UserSemaphore.hxx"

#include <catch_amalgamated.hpp>

#include <atomic>

TEST_CASE("Threading / Locks / Atomic")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    std::atomic<size_t> actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            ++actual;
        }
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / CriticalSection")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    CriticalSection cs{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            cs.With([&]
            {
                ++actual;
            });
        }
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / RecursiveCriticalSection")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    RecursiveCriticalSection cs{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        cs.With([&]
        {
            for (size_t i = 0; i < count; ++i)
            {
                cs.With([&]
                {
                    ++actual;
                });
            }
        });
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / ReaderWriterLock")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    ReaderWriterLock rwlock{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        rwlock.With([&]
        {
            for (size_t i = 0; i < count; ++i)
            {
                ++actual;
            }
        });
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / Semaphore")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    Semaphore semaphore{1};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            semaphore.Acquire();
            ++actual;
            semaphore.Release();
        }
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / Spinlock")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    Spinlock spinlock{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            spinlock.With([&]
            {
                ++actual;
            });
        }
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / UserCriticalSection")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    UserCriticalSection cs{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            cs.With([&]
            {
                ++actual;
            });
        }
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / UserReaderWriterLock")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    UserReaderWriterLock rwlock{};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        rwlock.With([&]
        {
            for (size_t i = 0; i < count; ++i)
            {
                ++actual;
            }
        });
    });

    REQUIRE(actual == expected);
}

TEST_CASE("Threading / Locks / UserSemaphore")
{
    constexpr size_t expected = 100'000;
    using namespace Anemone;

    UserSemaphore semaphore{1};
    size_t actual{};

    Parallel::For(expected, 6, [&](size_t index, size_t count)
    {
        (void)index;

        for (size_t i = 0; i < count; ++i)
        {
            semaphore.Acquire();
            ++actual;
            semaphore.Release();
        }
    });

    REQUIRE(actual == expected);
}
