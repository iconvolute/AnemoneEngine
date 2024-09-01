#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Initialize.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

#include <fmt/format.h>
#include <atomic>

AE_DECLARE_PROFILE(Main);
AE_DECLARE_PROFILE(Outer);
AE_DECLARE_PROFILE(Inner);

void TestTasking();

int main(int argc, char* argv[])
{
    Anemone::RuntimeInitializer runtime{argc, argv};

    auto const& processor = Anemone::System::GetProcessorProperties();

    AE_TRACE(Info, "PhysicalCores:       '{}'", processor.PhysicalCores);
    AE_TRACE(Info, "LogicalCores:        '{}'", processor.LogicalCores);
    AE_TRACE(Info, "PerformanceCores:    '{}'", processor.PerformanceCores);
    AE_TRACE(Info, "EfficiencyCores:     '{}'", processor.EfficiencyCores);
    AE_TRACE(Info, "CacheSizeLevel1:     '{}'", processor.CacheSizeLevel1);
    AE_TRACE(Info, "CacheSizeLevel2:     '{}'", processor.CacheSizeLevel2);
    AE_TRACE(Info, "CacheSizeLevel3:     '{}'", processor.CacheSizeLevel3);
    AE_TRACE(Info, "CacheSizeLevel4:     '{}'", processor.CacheSizeLevel4);
    AE_TRACE(Info, "CacheLineSize:       '{}'", processor.CacheLineSize);
    AE_TRACE(Info, "Name:                '{}'", processor.Name);
    AE_TRACE(Info, "Vendor:              '{}'", processor.Vendor);

    auto const& environment = Anemone::System::GetEnvironment();
    AE_TRACE(Info, "SystemVersionString: '{}'", environment.SystemVersion);
    AE_TRACE(Info, "SystemId:            '{}'", environment.SystemId);
    AE_TRACE(Info, "ExecutablePath:      '{}'", environment.ExecutablePath);
    AE_TRACE(Info, "StartupPath:         '{}'", environment.StartupPath);
    AE_TRACE(Info, "ComputerName:        '{}'", environment.ComputerName);
    AE_TRACE(Info, "UserName:            '{}'", environment.UserName);
    AE_TRACE(Info, "DesktopPath:         '{}'", environment.DesktopPath);
    AE_TRACE(Info, "DocumentsPath:       '{}'", environment.DocumentsPath);
    AE_TRACE(Info, "ProfilePath:         '{}'", environment.ProfilePath);
    AE_TRACE(Info, "TempPath:            '{}'", environment.TempPath);
    AE_TRACE(Info, "StartupTime:         '{}'", environment.StartupTime);

    {
        AE_PROFILE_SCOPE(Main);
        {
            AE_PROFILE_SCOPE(Inner);
            AE_TRACE(Warning, "Hello {}", Anemone::System::GetEnvironment().StartupTime);
            fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
            Anemone::Diagnostic::GetCurrentStackTrace([&](auto const& entry)
            {
                AE_TRACE(Warning, "{}", entry);
                return true;
            });
        }

        Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(500));
        {
            AE_PROFILE_SCOPE(Outer);
            Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(500));
            {
                AE_PROFILE_SCOPE(Inner);
                Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(500));

                TestTasking();

                AE_TRACE(Critical, "Crashed!?");
                //std::atomic<int>* p = nullptr;
                //p->store(42);
                //AE_ASSERT(false, "This is a test of the emergency broadcast system: {}", 42);
            }
            Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(500));
        }
        Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(500));
    }

    return 0;
}


AE_DECLARE_PROFILE(ReportTimeUsage);

template <typename T>
void ReportTimeUsage(T t)
{
    AE_PROFILE_SCOPE(ReportTimeUsage);

    auto const started = Anemone::Instant::Now();
    t();
    auto const elapsed = started.QueryElapsed();
    fmt::println("Time elapsed: {}", elapsed);
}

AE_DECLARE_PROFILE(ParallelFor4);

#if false
#define TRACEPOINT(format, ...) AE_TRACE(Verbose, "({}): {} at {:x}: " format, Anemone::DateTime::Now(), __func__, Anemone::Threading::GetThisThreadId().Value __VA_OPT__(, ) __VA_ARGS__);

#else
#define TRACEPOINT(...)
#endif


#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Crypto/Sha512.hxx"

class MyFancyTaskWithAlignment : public Anemone::Tasks::Task
{
public:
    alignas(64) std::array<uint8_t, 64> m_Data{};
    Anemone::Crypto::Sha512Hash m_Output{};

public:
    MyFancyTaskWithAlignment() = default;

    void OnExecute() override
    {
        AE_PROFILE_SCOPE(ParallelFor4);
        TRACEPOINT("started: ptr={}, id={}", fmt::ptr(this), this->GetId());
#if 0
        using namespace Anemone::Crypto;

        auto hash = sha512_from_buffer(std::as_bytes(std::span{this->m_Data}));

        size_t limit = 1'000'00;
        if (this->GetId() % 2 == 0)
        {
            limit *= 13;
            limit /= 10;
        }
        else
        {
            limit *= 8;
            limit /= 10;
        }

        for (size_t i = 0; i < limit; ++i)
        {
            hash = sha512_from_buffer(std::as_bytes(std::span{hash}));
        }

        this->m_Output = hash;
#else
        Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(100));
#endif

        TRACEPOINT("finished: ptr={}, id={}", fmt::ptr(this), this->GetId());
    }
};

AE_DECLARE_PROFILE(EnumerateConsoleVariables);
AE_DECLARE_PROFILE(ParallelFor1);
AE_DECLARE_PROFILE(ParallelFor2);
AE_DECLARE_PROFILE(ParallelFor3);

#include "AnemoneRuntime/Threading/CriticalSectionSlim.hxx"
#include "AnemoneRuntime/Threading/ConcurrentAccess.hxx"
#include "AnemoneRuntime/Tasks/ParallelFor.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

void TestTasking()
{
    {
        using namespace Anemone::Tasks;
        using namespace Anemone::Threading;

        {
#ifdef UNSYNCED1
            CriticalSectionSlim scs{};
            ConcurrentAccess ca{};
            size_t processed = 0;
#endif
            constexpr size_t total = 128 * 1024 + 63;
            ReportTimeUsage([&]
            {
                ParallelFor(
                    total,
                    16,
                    [&](size_t first, size_t last)
                {
                    (void)first;
                    (void)last;
                    AE_PROFILE_SCOPE(ParallelFor1);
#ifdef UNSYNCED1
                    {
                        UniqueLock lock{scs};
                        UniqueLock lock2{ca};
                        processed += (last - first);
                    }
#endif
                    TRACEPOINT("ParallelFor: [{}:{}): {}", first, last, last - first);
                    Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(50));
                },
                    [&](size_t count)
                {
                    AE_PROFILE_SCOPE(ParallelFor2);
                    (void)count;
#ifdef UNSYNCED1
                    AE_ASSERT(processed == count);
#endif
                    TRACEPOINT("ParallelFor: done [0:{})", count);
                    Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(10));
                });
            });
#ifdef UNSYNCED1
            AE_FATAL(processed == total);
#endif
        }
        {
            CriticalSectionSlim scs{};
            size_t processed = 0;
            constexpr size_t total = 128 * 1024 + 63;

            ReportTimeUsage([&]
            {
                ParallelFor(
                    total,
                    1,
                    [&](size_t first, size_t last)
                {
                    AE_PROFILE_SCOPE(ParallelFor3);
                    {
                        UniqueLock lock{scs};
                        processed += (last - first);
                    }
                    Anemone::Threading::SleepThread(Anemone::Duration::FromMilliseconds(5));
                },
                    [&](size_t count)
                {
                    AE_PROFILE_SCOPE(ParallelFor3);
                    (void)count;
                    AE_ASSERT(processed == count);
                    TRACEPOINT("ParallelFor: done [0:{})", count);
                });
            });
            AE_ASSERT(processed == total);
        }
        {
            AwaiterHandle barrier{new Awaiter{}};
            AwaiterHandle dummy{new Awaiter{}};

            TaskHandle h1{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*h1, dummy, barrier);
            TaskHandle h2{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*h2, dummy, barrier);
            dummy = {};
            h1 = {};
            h2 = {};

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(barrier);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::Threading::SleepThread(Anemone::Duration::FromSeconds(10));
        TRACEPOINT("<< waiting for workers to finish mid test");

        {
            AwaiterHandle barrier{new Awaiter{}};
            AwaiterHandle dummy{new Awaiter{}};

            for (size_t i = 0; i < 18; ++i)
            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                GTaskScheduler->Dispatch(*h1, dummy, barrier);
            }

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(barrier);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::Threading::SleepThread(Anemone::Duration::FromSeconds(10));
        TRACEPOINT("<< waiting for workers to finish mid test");

        {
            AwaiterHandle leaf{new Awaiter{}};
            AwaiterHandle trunk{new Awaiter{}};
            AwaiterHandle root{new Awaiter{}};

            TaskHandle h1{new MyFancyTaskWithAlignment()};
            TaskHandle h2{new MyFancyTaskWithAlignment()};

            GTaskScheduler->Dispatch(*h1, leaf, trunk);

            GTaskScheduler->Dispatch(*h2, trunk, root);

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::Threading::SleepThread(Anemone::Duration::FromSeconds(2));
        TRACEPOINT("<< waiting for workers to finish mid test");
        {
            AwaiterHandle root{new Awaiter{}};
            AwaiterHandle trunk1{new Awaiter{}};
            AwaiterHandle trunk2{new Awaiter{}};
            AwaiterHandle leaf{new Awaiter{}};

            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                TaskHandle h2{new MyFancyTaskWithAlignment()};
                TaskHandle h3{new MyFancyTaskWithAlignment()};
                GTaskScheduler->Dispatch(*h1, leaf, trunk1);
                GTaskScheduler->Dispatch(*h2, trunk1, trunk2);
                GTaskScheduler->Dispatch(*h3, trunk2, root);
            }

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });

            AE_TRACE(Verbose, "------------------------------------------");
        }

        AE_TRACE(Verbose, "-- should wait here ----------------------------------------");
        {
            AwaiterHandle awaiters[65]{};

            for (AwaiterHandle& awaiter : awaiters)
            {
                awaiter = new Awaiter{};
            }

            TaskHandle tasks[64]{};

            for (TaskHandle& task : tasks)
            {
                task = new MyFancyTaskWithAlignment();
            }

            for (size_t i = 0; i < 64; ++i)
            {
                GTaskScheduler->Dispatch(*tasks[i], awaiters[i], awaiters[i + 1]);
            }

            // AwaiterHandle accumulate{new Awaiter{}};
            // AwaiterHandle wait{new Awaiter{}};
            AwaiterHandle total = awaiters[64];

            //{
            //    for (size_t i = 0; i < 64; ++i)
            //    {
            //        TaskHandle h1{new MyFancyTaskWithAlignment()};
            //        DispatchTask(*h1, wait, accumulate);
            //        total = accumulate;
            //        // accumulate = std::exchange(wait, AwaiterHandle{new Awaiter{}});
            //        wait = std::exchange(accumulate, AwaiterHandle{new Awaiter{}});
            //    }
            //}

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(total);
            });
        }

        AE_TRACE(Verbose, "------------------------------------------");
        {
            AwaiterHandle ca1{new Awaiter()};
            AwaiterHandle ca2{new Awaiter()};
            AwaiterHandle ca3{new Awaiter()};

            AwaiterHandle cw1{new Awaiter()};
            AwaiterHandle cw2{new Awaiter()};
            AwaiterHandle cw3{new Awaiter()};

            AwaiterHandle root1{};
            AwaiterHandle root2{};
            AwaiterHandle root3{};

            for (size_t i = 0; i < 64; ++i)
            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                TaskHandle h2{new MyFancyTaskWithAlignment()};
                TaskHandle h3{new MyFancyTaskWithAlignment()};

                GTaskScheduler->Dispatch(*h1, cw1, ca1);
                GTaskScheduler->Dispatch(*h2, cw2, ca2);
                GTaskScheduler->Dispatch(*h3, cw3, ca3);

                root1 = ca1;
                root2 = ca2;
                root3 = ca3;

#if 0 // parallel
                            ca1 = std::exchange(cw1, AwaiterHandle{new Awaiter()});
                            ca2 = std::exchange(cw2, AwaiterHandle{new Awaiter()});
                            ca3 = std::exchange(cw3, AwaiterHandle{new Awaiter()});
#else // serial
                cw1 = std::exchange(ca1, AwaiterHandle{new Awaiter()});
                cw2 = std::exchange(ca2, AwaiterHandle{new Awaiter()});
                cw3 = std::exchange(ca3, AwaiterHandle{new Awaiter()});
#endif
            }

            AwaiterHandle root{new Awaiter{}};
            TaskHandle r1{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r1, root1, root);
            TaskHandle r2{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r2, root2, root);
            TaskHandle r3{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r3, root3, root);

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });
        }
        AE_TRACE(Verbose, ">> finish ------------------------------------------");
    }
}
