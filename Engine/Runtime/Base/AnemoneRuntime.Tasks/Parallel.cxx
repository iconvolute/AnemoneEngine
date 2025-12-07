#include "AnemoneRuntime.Tasks/Parallel.hxx"
#include "AnemoneRuntime.Tasks/TaskScheduler.hxx"
#include <algorithm>

namespace Anemone
{
    void Parallel::For(
        size_t count,
        size_t batch,
        FunctionRef<void(size_t index, size_t count)> callback,
        FunctionRef<void(size_t count)> finalize,
        size_t workers,
        TaskPriority priority)
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

        TaskScheduler& taskScheduler = TaskScheduler::Get();

        struct Partitioner final
        {
            FunctionRef<void(size_t index, size_t count)> Callback{};
            size_t Count{};
            size_t Batch{};

            std::atomic<size_t> CurrentIndex{};

            bool Partition(size_t& first, size_t& last)
            {
                size_t const batch = this->Batch;
                size_t const count = this->Count;

                size_t current = this->CurrentIndex.load(std::memory_order::relaxed);

                while (true)
                {
                    if (current >= count)
                    {
                        // No more work.
                        return false;
                    }

                    size_t next;

                    if (batch > (count - current))
                    {
                        next = count;
                    }
                    else
                    {
                        next = current + batch;
                    }

                    if (this->CurrentIndex.compare_exchange_weak(
                            current,
                            next,
                            std::memory_order::acquire,
                            std::memory_order::relaxed))
                    {
                        // Successfully claimed the batch.
                        first = current;
                        last = next;
                        return true;
                    }
                }
            }

            void Execute()
            {
                size_t first;
                size_t last;

                while (this->Partition(first, last))
                {
                    this->Callback(first, last - first);
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
                this->m_Partitioner.Execute();
            }
        };

        // Number of worker threads in pool.
        auto workersCount = std::max<size_t>(1, taskScheduler.GetThreadsCount());

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
        workers = std::min<size_t>(workers, std::min<size_t>(slices, maximumThreads));

        // Spawn tasks to (hopefully) saturate thread pool.
        TaskHandle children[32];

        // Create partitioner for given range.
        Partitioner partitioner{
            callback,
            count,
            batch,
        };

        // Fork/Join awaiter.
        TaskAwaiterHandle joinCounter = MakeReference<TaskAwaiter>();
        TaskAwaiterHandle forkCounter = MakeReference<TaskAwaiter>();

        // Spawn tasks.
        for (auto i = 0uz; i < workers; ++i)
        {
            children[i] = MakeReference<ParallelForTask>(partitioner);

            taskScheduler.Schedule(*children[i], joinCounter, forkCounter, priority);
        }

        // Contribute with executing tasks.
        partitioner.Execute();

        // Wait for counter.
        // TODO: for diagnostic purposes, could we return number of processed tasks during awaiting?
        taskScheduler.Wait(joinCounter);

        // Finalize range.
        finalize(count);
    }
}
