#pragma once
#include <algorithm>

#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    struct Parallel final
    {
        Parallel() = delete;

        template <typename Callback = void(size_t index, size_t count), typename Finalize = void(size_t count)>
        static void For(
            size_t count,
            size_t batch,
            Callback&& callback,
            Finalize&& finalize,
            size_t workers = 0,
            TaskPriority priority = TaskPriority::Inherited)
        {
            AE_ASSERT(batch != 0);
            if (count == 0)
            {
                //
                // Make sure to call the finalize callback when the count is zero.
                //

                finalize(count);
                return;
            }

            class Partitioner final
            {
            public:
                size_t m_Count;
                size_t m_Batch;
                std::atomic<size_t> m_Index{};
                Callback& m_Callback;

            public:
                Partitioner(
                    size_t count,
                    size_t batch,
                    Callback& callback)
                    : m_Count{count}
                    , m_Batch{batch}
                    , m_Callback{callback}
                {
                }
                Partitioner(Partitioner const&) = delete;
                Partitioner(Partitioner&&) = delete;
                Partitioner& operator=(Partitioner const&) = delete;
                Partitioner& operator=(Partitioner&&) = delete;
                ~Partitioner() = default;

                bool Partition(size_t& first, size_t& last)
                {
                    size_t const current = this->m_Index.fetch_add(this->m_Batch, std::memory_order::acquire);

                    if (current < this->m_Count)
                    {
                        first = current;
                        last = std::min<size_t>(current + this->m_Batch, this->m_Count);
                        return true;
                    }

                    return false;
                }

                void ExecuteCallbacks()
                {
                    size_t first;
                    size_t last;

                    while (this->Partition(first, last))
                    {
                        this->m_Callback(first, last - first);
                    }
                }
            };

            class ParallelForTask final : public Task
            {
            private:
                Partitioner& m_Partitioner;

            public:
                explicit ParallelForTask(Partitioner& parent)
                    : m_Partitioner{parent}
                {
                }

                ParallelForTask(ParallelForTask const&) = delete;
                ParallelForTask(ParallelForTask&&) = delete;
                ParallelForTask& operator=(ParallelForTask const&) = delete;
                ParallelForTask& operator=(ParallelForTask&&) = delete;
                ~ParallelForTask() override = default;

            protected:
                void OnExecute() override
                {
                    this->m_Partitioner.ExecuteCallbacks();
                }
            };

            // Number of worker threads in pool.
            auto workersCount = std::max<size_t>(1, TaskScheduler::GetWorkerCount());

            if (workers == 0)
            {
                // Caller did not specify the number of threads.
                workers = workersCount;
            }
            else
            {
                // Adjust the number of threads to the number of workers.
                workers = std::min<size_t>(workers, workersCount);
            }

            // Estimated number of slices.
            size_t const slices = (count / batch) + 1;

            // TODO: Promote this to some configurable constant.
            constexpr auto maximumThreads = 32uz;

            // Adjust the number of threads to the number of slices.
            workers = std::min<size_t>({workers, slices, maximumThreads});

            // Spawn tasks to (hopefully) saturate thread pool.
            TaskHandle children[32];

            // Create partitioner for given range.
            Partitioner partitioner{
                count,
                batch,
                callback,
            };

            // Fork/Join awaiter.
            AwaiterHandle joinCounter = new Awaiter{};
            AwaiterHandle forkCounter = new Awaiter{};

            // Spawn tasks.
            for (auto i = 0uz; i < workers; ++i)
            {
                children[i] = new ParallelForTask(partitioner);

                TaskScheduler::Dispatch(*children[i], joinCounter, forkCounter, priority);
            }

            // Contribute with executing tasks.
            partitioner.ExecuteCallbacks();

            // Wait for counter.
            // TODO: for diagnostic purposes, could we return number of processed tasks during awaiting?
            TaskScheduler::Wait(joinCounter);

            // Finalize range.
            std::forward<Finalize>(finalize)(count);
        }
    };
}
