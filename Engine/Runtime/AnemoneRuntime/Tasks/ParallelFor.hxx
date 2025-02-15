#pragma once
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

namespace Anemone::Tasks
{
    template <typename TCallback = void(size_t, size_t), typename TFinalize = void(size_t)>
    void ParallelFor(
        size_t count,
        size_t batch,
        TCallback&& callback,
        TFinalize&& finalize,
        size_t threads = 0,
        TaskPriority priority = TaskPriority::Inherited)
    {
        AE_ASSERT(batch != 0);

        if (count == 0)
        {
            // Make sure to call the finalize callback when the count is zero.
            finalize(count);
            return;
        }

        class Partitioner final
        {
        public:
            size_t m_Count;
            size_t m_Batch;
            std::atomic<size_t> m_Index{};
            TCallback m_Callback;

        public:
            Partitioner(
                size_t count,
                size_t batch,
                TCallback&& callback)
                : m_Count{count}
                , m_Batch{batch}
                , m_Callback{std::forward<TCallback>(callback)}
            {
            }

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
                    this->m_Callback(first, last);
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

        protected:
            void OnExecute() override
            {
                this->m_Partitioner.ExecuteCallbacks();
            }
        };

        // Number of worker threads in pool.
        //
        // This takes into account the main thread as a worker.
        auto workersCount = std::max<size_t>(1, GTaskScheduler->GetWorkerCount() + 1);

        if (threads == 0)
        {
            // Caller did not specify the number of threads.
            threads = workersCount;
        }
        else
        {
            // Adjust the number of threads to the number of workers.
            threads = std::min<size_t>(threads, workersCount);
        }

        // Estimated number of slices.
        size_t const slices = (count / batch) + 1;

        // TODO: Promote this to some configurable constant.
        constexpr auto maximumThreads = 32uz;

        // Adjust the number of threads to the number of slices.
        threads = std::min<size_t>(std::min<size_t>(threads, slices), maximumThreads);

        // Spawn tasks to (hopefully) saturate thread pool.
        TaskHandle children[32];

        // Create partitioner for given range.
        Partitioner partitioner{
            count,
            batch,
            std::forward<TCallback>(callback),
        };

        // Fork/Join awaiter.
        AwaiterHandle joinCounter = new Awaiter{};
        AwaiterHandle forkCounter = new Awaiter{};

        // Spawn tasks.
        for (auto i = 0uz; i < threads; ++i)
        {
            children[i] = new ParallelForTask(partitioner);

            GTaskScheduler->Dispatch(*children[i], joinCounter, forkCounter, priority);
        }

        // Contribute with executing tasks.
        partitioner.ExecuteCallbacks();

        // Wait for counter.
        // TODO: for diagnostic purposes, could we return number of processed tasks during awaiting?
        GTaskScheduler->Wait(joinCounter);

        // Finalize range.
        std::forward<TFinalize>(finalize)(count);
    }
}
